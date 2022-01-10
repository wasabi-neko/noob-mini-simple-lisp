#include "error_handling.hpp"
#include "type.hpp"
#include "runtime.hpp"

void core_dump(env_t *env) {
    // TODO:
    return;
}
void trace_back(env_t *env) {
    printf("Trace Back:\n");
    dump_data_stack(env);
    dump_func_stack(env);
    // TODO:
    return;
}

void exit_interpreter(env_t *env) { 
    fflush(stdout);
    exit(-1);
}

void raise_type_error(env_t *env, enum var_types type_expect, var_t the_var) {
    trace_back(env);
    printf("Type Error: "); print_type_str(type_expect); printf("expect, but ");
    print_var_val(the_var);
    printf("received.\n");
    exit_interpreter(env);
}

void raise_not_callable_error(env_t *env, var_t the_var) {
    trace_back(env);
    printf("Type Error: the value "); print_var_val(the_var);
    printf("is not called-able.\n");
    exit_interpreter(env);
}

void raise_name_error(env_t *env, std::string *symbol) {
    trace_back(env);
    printf("Name Error: symbol \'%s\' is undefine\n", symbol->data());
    exit_interpreter(env);
}

void raise_argc_error(env_t *env, func_t *func, int argc_given) {
    trace_back(env);
    printf("Argument numbers Error: ");
    int argc = func->argc;
    if (argc < 0) {
        printf("need at least %d arguments", (argc + 1) * -1);
    } else {
        printf("need %d arguments", argc);
    }
    printf(", but %d received\n", argc_given);
    exit_interpreter(env);
}

void raise_stackoverflow_error(env_t *env) {
    trace_back(env);
    printf("Stack Overflow");
    exit_interpreter(env);
}

void assert_argc(env_t *env, func_t *func, int argc_given) {
    if (func->argc < 0) {
        if (argc_given < (func->argc + 1) * -1)
            raise_argc_error(env, func, argc_given);
    } else {
        if (argc_given != func->argc)
            raise_argc_error(env, func, argc_given);
    }
}

void assert_type(env_t *env, enum var_types type_expect, var_t the_var) {
    if (!type_check(the_var, type_expect)) {
        raise_type_error(env, type_expect, the_var);
    }
}