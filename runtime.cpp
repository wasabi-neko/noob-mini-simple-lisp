#include "runtime.hpp"
#include "native_func.hpp"

// -----------------------------------------------------------------------------
// Stack Dump methods
// -----------------------------------------------------------------------------

void dump_data_stack(env_t *env) {
    int rsp = env->data_stack.rsp;
    int rbp = env->data_stack.rbp;
    printf("dump data stack:");
    printf("(rbp: %d, rsp: %d)\n", rbp, rsp);

    for (int i = 0; i <= rsp; i++) {
        printf("%d [", i);
        print_var_val(env->data_stack.stack[i]);
        printf("]\n");
    }
    printf("\n");
}

void dump_func_stack(env_t *env) {
    int rsp = env->func_stack.rsp;
    
    printf("dump func stack: ");
    printf("(rbp: %d, rsp: %d)\n", 0, rsp);

    for (int i = 0; i <= rsp; i++) {
        printf("%d [", i);
        std::string *name = env->func_stack.stack[i]->name;
        if (name != NULL) {
            printf("%s", name->data());
        } else {
            printf("no name");
        }

        printf("]\n");
    }
    printf("\n");
}
// -----------------------------------------------------------------------------
// Data_stack
// -----------------------------------------------------------------------------

void push_stack(env_t * env, var_t val) {
    if (env->data_stack.rsp >= RUNTIME_STACK_SIZE_MAX) {
        //! error
        printf("stack overflow");
        return;
    }
    env->data_stack.rsp += 1;
    env->data_stack.stack[env->data_stack.rsp] = val;
    return;
}

var_t pop_stack(env_t *env) {
    if (env->data_stack.rsp < 0) {
        // ! error
        printf("no content in stack can be popped");
        return {};
    }
    var_t val = env->data_stack.stack[env->data_stack.rsp];
    env->data_stack.rsp -= 1;
    return val;
}

var_t get_local_var(env_t *env, int offset) {
    int index = env->data_stack.rbp + offset;
    if (index < 0 || index >= RUNTIME_STACK_SIZE_MAX) {
        // ! error
        printf("illigle memory acceess");
        return {};
    }

    return env->data_stack.stack[index];
}

void create_stack_frame(env_t *env) {
    var_t old_rbp =  set_var_val(lisp_int32, {.lisp_int32 = env->data_stack.rbp});
    push_stack(env, old_rbp);
    env->data_stack.rbp = env->data_stack.rsp;
    return;
}

void release_stack_frame(env_t *env) {
    env->data_stack.rsp = env->data_stack.rbp;                 // mov rsp rbp
    env->data_stack.rbp = pop_stack(env).lisp_int32;           // pop rbp
    return;
}


// -----------------------------------------------------------------------------
// Func_stack
// -----------------------------------------------------------------------------

func_t *get_top_func(env_t *env) {
    int index = env->func_stack.rsp;

    if (index < 0 || index >= RUNTIME_FUNC_CALL_STACK_MAX) {
        // ! error
        printf("top func access error");
        return NULL;
    }

    return env->func_stack.stack[index];
}

// push function and add static link to it
// * you should move rbp to ready state before use this method
void add_func_call_stack(env_t *env, func_t *func_template) {
    if (env->func_stack.rsp >= RUNTIME_FUNC_CALL_STACK_MAX) {
        // ! error
        printf("funcall out of stack");
        return;
    }
    func_t *instance = clone_func(func_template);
    instance->static_parent = NULL;
    func_t *target = get_top_func(env);

    // * search static parent
    // the parent scope level should be smaller clone
    while (target != NULL) {
        if (target->scope_level == instance->scope_level) {
            instance->static_parent = target->static_parent;
            break;
        }
        if (target->scope_level < instance->scope_level) {
            instance->static_parent = target;
            break;
        }
        if (target->scope_level > instance->scope_level) {
            target = target->static_parent;
            if (target == target->static_parent) {
                break;
            }
        }
    }

    if (instance->static_parent == NULL) {
        // !error
        printf("cannot find static parent for");
        if (instance->name == NULL) {
            printf("NULL\n");
        } else {
            printf("%s", instance->name->data());
        } 
        return;
    }

    // * get runtime rbp in stack frame
    instance->runtime_rbp = env->data_stack.rbp;

    env->func_stack.rsp += 1;
    env->func_stack.stack[env->func_stack.rsp] = instance;
    return;
}

void pop_func_stack(env_t *env) {
    if (env->func_stack.rsp <= 0) {
        // !error
        printf("cannot pop funcal stack");
        return;
    }
    free_func(env->func_stack.stack[env->func_stack.rsp]);      // free runtime function instance 
    env->func_stack.rsp -= 1;                                   // pop
    return;
}

// -----------------------------------------------------------------------------
// Runtime Methods
// -----------------------------------------------------------------------------
var_t ask_symbol(env_t *env, std::string* id_name) {
    func_t *target = env->func_stack.stack[env->func_stack.rsp];    // from top
    int offset;

    while (target != NULL) {
        // if target don't have id_map, or the id cannot be found in the id_map
        if (target->id_map == NULL || target->id_map->count(*id_name) == 0) {
            target = target->static_parent;
            continue;
        }

        offset = target->id_map->at(*id_name);
        break;
    }

    // if not found
    if (target == NULL) {
        // ! error
        printf("symbol \'%s\' not found", id_name->data());
        return {};
    }

    int index = target->runtime_rbp + offset;
    return env->data_stack.stack[index];
}

var_t interpret_ast(AST_node *root, env_t *env, bool allow_exp_arg) {
    create_stack_frame(env);
    
    printf("start push list\n");
    dump_data_stack(env);
    dump_func_stack(env);
    printf("\n======================\n");

    // * First step:
    // * push all list into stack
    AST_node *node = root;
    while (node != NULL) {
        print_node(node);
        printf("\n");
        var_t val_to_push;

        if (node->child != NULL) {
            // if the node is a compound expression
            if (allow_exp_arg) {
                // just push the AST node into the stack as the argument
                val_to_push.ast_node_ptr = node;    // mask of pointer is 0x00, so it's fine to not to use set_val function
            } else {
                // interpret the node and push the result into  stack
                // peek next to know should it allow_exp_arg
                bool next_allow_expr = false;
                if (node->child->child == NULL && type_check(node->child->val, lisp_ptr)) {
                    next_allow_expr = node->child->val.func_ptr->allow_exp_arg;
                }
                val_to_push = interpret_ast(node->child, env, next_allow_expr);     // interpret the node
            }
        } else if (type_check(node->val, lisp_symbol)) {
            // if the node is a symbol
            val_to_push = ask_symbol(env, node->name);

        } else {
            // just single value
            val_to_push = node->val;
        }

        push_stack(env, val_to_push);
        node = node->next;
    }

    //* Step 2:
    //* function evoke
    var_t first = get_local_var(env, 1);                    // fetch first item and do type checking

    // ! test function
    printf("end push list\n");
    dump_data_stack(env);
    dump_func_stack(env);
    printf("\n======================\n");

    if (!type_check(first, lisp_ptr)) {
        // ! error
        printf("the first item is not callable\n");
        return {};
    }

    add_func_call_stack(env, (func_t*)first.lisp_ptr);   // clone this function and push into funccall stack
    func_t *instance = get_top_func(env);

    var_t result;
    if (instance->is_native) {
        instance->body.native_body(instance, env);   // natives should release frame inside
        result = env->result;                        // fetch result from result register in env
    } else {
        result = interpret_ast(instance->body.ast_body, env, false);
    }

    //* Step 3:
    //* return function
    release_stack_frame(env);
    pop_func_stack(env);
    return result;
}

void execute_main(AST_node *root, env_t *env) {
    func_t *main_template = (func_t*)&LISP_NATIVE_FUNC_MAIN_INFO;
    main_template->scope_level = 0;
    main_template->name = new std::string("lisp_main");
    func_t *main_instance = clone_func(main_template);

    // prepare init env
    env->data_stack.rsp = -1;
    env->data_stack.rbp = -1;
    env->func_stack.rsp = -1;
    // create_stack_frame(env);

    // // push main func into stack (the main func aka exit will be executed when the program is over)
    // push_stack(env, set_var_val(lisp_ptr, {.func_ptr = main_instance}));

    // push func manually. don't search for static_parent 
    main_instance->static_parent = main_instance;
    env->func_stack.rsp += 1;
    env->func_stack.stack[env->func_stack.rsp] = main_instance;

    dump_data_stack(env);
    dump_func_stack(env);
    printf("\n======================\n");

    AST_node *exit_node = create_ast_nf_node(&LISP_NATIVE_FUNC_MAIN_INFO, NULL);
    exit_node->next = root;
    interpret_ast(exit_node, env, false);
    return;
}