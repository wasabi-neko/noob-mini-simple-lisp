#include "runtime.hpp"

// -----------------------------------------------------------------------------
// Data_stack
// -----------------------------------------------------------------------------

void push_stack(env_t * env, var_t val) {
    if (env->data_stack.rsp + 1 >= RUNTIME_STACK_SIZE_MAX) {
        //! error
        printf("stack overflow");
        return;
    }
    env->data_stack.stack[env->data_stack.rsp] = val;
    env->data_stack.rsp += 1;
    return;
}

var_t pop_stack(env_t *env) {
    if (env->data_stack.rsp == 0) {
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
    if (index <= 0 || index >= RUNTIME_STACK_SIZE_MAX) {
        // ! error
        printf("illigle memory acceess");
        return {};
    }

    return env->data_stack.stack[index];
}

void create_stack_frame(env_t *env) {
    push_stack(env, {.lisp_int32 = env->data_stack.rbp});       // push rbp
    // with the recursive interpere method, i don't need to push rip
    // the rbp will be moved to the position rightnow later on
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
    int index = env->func_stack.rsp - 1;

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
    if (env->func_stack.rsp + 1 >= RUNTIME_FUNC_CALL_STACK_MAX) {
        // ! error
        printf("funcall out of stack");
        return;
    }
    func_t *instance = clone_func(func_template);
    func_t *target = env->func_stack.stack[env->func_stack.rsp];

    // * search static parent
    // the parent scope level should be smaller clone
    while (target != NULL) {
        if (target->scope_level == instance->scope_level) {
            instance->static_parent = target->static_parent;
            break;
        }
        if (target->scope_level < instance->scope_level) {
            instance->static_parent = instance;
            break;
        }
        if (target->scope_level > instance->scope_level) {
            target = target->static_parent;
        }
    }

    if (instance->static_parent == NULL) {
        // !error
        printf("cannot find static parent for %s\n", instance->name->data());
        return;
    }

    // * get runtime rbp in stack frame
    instance->runtime_rbp = env->data_stack.rbp;

    env->func_stack.stack[env->func_stack.rsp + 1] = instance;
    env->func_stack.rsp += 1;
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

