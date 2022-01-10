#include "runtime.hpp"
#include "native_func.hpp"
#include "error_handling.hpp"

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
        printf("]");
        printf("\n");
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
    if (env->data_stack.rsp >= RUNTIME_STACK_SIZE_MAX)
        raise_stackoverflow_error(env);
    env->data_stack.rsp += 1;
    env->data_stack.stack[env->data_stack.rsp] = val;
    return;
}

var_t pop_stack(env_t *env) {
    if (env->data_stack.rsp < 0) {
        raise_stackoverflow_error(env);
    }
    var_t val = env->data_stack.stack[env->data_stack.rsp];
    env->data_stack.rsp -= 1;
    return val;
}

var_t get_local_var(env_t *env, int offset) {
    int index = env->data_stack.rbp + offset;
    if (index < 0 || index >= RUNTIME_STACK_SIZE_MAX) {
        raise_stackoverflow_error(env);
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
        raise_stackoverflow_error(env);
    }

    return env->func_stack.stack[index];
}

// TODO: there's seems some sort of bug in my static_scope searching
// push function and add static link to it
void add_func_call_stack(env_t *env, func_t *func_template) {
    if (env->func_stack.rsp >= RUNTIME_FUNC_CALL_STACK_MAX) {
        raise_stackoverflow_error(env);
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
        }
    }

    if (instance->static_parent == NULL) {
        // !error
        printf("ERROR cannot find static parent for");
        if (instance->name == NULL) {
            printf("NULL\n");
        } else {
            printf("%s\n", instance->name->data());
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
    var_memory_t var_memory;

    while (target != NULL) {
        // if target don't have id_map, or the id cannot be found in the id_map
        if (target->id_map == NULL || target->id_map->count(*id_name) == 0) {
            target = target->static_parent;
            continue;
        }

        var_memory = target->id_map->at(*id_name);
        break;
    }

    // if not found
    if (target == NULL) {
        raise_name_error(env, id_name);
    }

    var_t result;
    if (var_memory.is_dynamic) {
        result = var_memory.memory.dyn_var;
    } else {
        int index = target->runtime_rbp + var_memory.memory.offset;
        result = env->data_stack.stack[index];
    }

    return result;
}

var_t get_runtime_var_val(env_t *env, var_t var, enum var_types type) {
    var_t true_value = var;
    if (type_check(var, lisp_symbol)) {
        true_value  = ask_symbol(env, get_symbol_ptr(var));
    } else if (type_check(var, lisp_ast_ptr)) {
        interpret_ast(get_ast_ptr(var)->child, env);
        true_value = env->result;
    }

    assert_type(env, type, true_value);
    return true_value;
}

var_t interpret_ast(AST_node *root, env_t *env) {
    if (root == NULL) {
        return set_var_val(lisp_nil, {._content = 0});
    }

    // * First step:
    // * push all list into stack
    create_stack_frame(env);
    AST_node *node = root;

    // * Preprocess: 
    // * eval the first item in list. to check is it call-able and fetch it's property
    // eval firs_val if it is compund expression
    bool allow_exp_arg;
    var_t first_val = get_runtime_var_val(env, node->val, lisp_any);
    // check call-able or raise error
    if (type_check(first_val, lisp_ptr)) {
        allow_exp_arg = first_val.func_ptr->allow_exp_arg;
    } else {
        raise_not_callable_error(env, first_val);
    }

    // push into stack
    push_stack(env, first_val);
    node = node->next;

    #ifdef DEBUG
    printf("start push list\n");
    printf("allow exp arg: %d\n", allow_exp_arg);
    dump_data_stack(env);
    dump_func_stack(env);
    printf("======================\n\n");
    #endif

    // For all item in the list, push them into stack
    while (node != NULL) {
        #ifdef DEBUG
        printf("next_node: ");
        print_node(node);
        printf("\n");
        #endif
        var_t val_to_push;

        if (allow_exp_arg) {
            val_to_push = node->val;
        } else {
            val_to_push = get_runtime_var_val(env, node->val, lisp_any);
        }
        push_stack(env, val_to_push);
        node = node->next;
    }

    //* Step 2:
    //* function evoke
    var_t first = get_local_var(env, 1);                    // fetch first item and do type checking

#ifdef DEBUG
    printf("end push list\n");
    dump_data_stack(env);
    dump_func_stack(env);
    printf("======================\n\n");
#endif

    if (!type_check(first, lisp_ptr)) {
        raise_not_callable_error(env, first);
    }

    add_func_call_stack(env, (func_t*)first.lisp_ptr);   // clone this function and push into funccall stack
    func_t *instance = get_top_func(env);

    var_t result;
    if (instance->is_native) {
        instance->body.native_body(instance, env);   // natives should release frame inside
        result = env->result;                        // fetch result from result register in env
    } else {
        result = interpret_ast(instance->body.ast_body, env);
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
    main_template->name = new std::string("lisp_main_func_scope");
    func_t *global_scope = clone_func(main_template);

    // prepare init env
    env->data_stack.rsp = -1;
    env->data_stack.rbp = -1;
    env->func_stack.rsp = -1;

    // push func manually. don't search for static_parent 
    global_scope->static_parent = NULL;
    env->func_stack.rsp += 1;
    env->func_stack.stack[env->func_stack.rsp] = global_scope;

    AST_node *exit_node = create_ast_nf_node(&LISP_NATIVE_FUNC_MAIN_EXIT_INFO, NULL);
    exit_node->next = root;

#ifdef DEBUG
    graph_AST(exit_node);
    dump_data_stack(env);
    dump_func_stack(env);
    printf("======================\n\n");
#endif

    interpret_ast(exit_node, env);
    return;
}