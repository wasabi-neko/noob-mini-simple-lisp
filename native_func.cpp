#include "native_func.hpp"
#include "runtime.hpp"
#include "type.hpp"

AST_node *create_ast_nf_node(const func_t *native_info, func_t *parent) {
    var_t fun_ptr = set_var_val(lisp_ptr, {.lisp_ptr = (void*)native_info});
    AST_node *node = new_ast_node(native_info->name, fun_ptr);
    return node;
}

void LISP_NATIVE_FUNC_BODY_MAIN(func_t *self, env_t *env) {
    printf("EXIT");
    exit(0);
    return;
}

#define DEFINE_SINGLE_OPERATION_FUNC_BODY(name, type, lambda)                         \
void LISP_NATIVE_FUNC_BODY##name (func_t *self, env_t *env) {                         \
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;                   \
    if (self->argc != -1 && argc_given != self->argc) {                               \
        /*// ! error*/                                                                \
        printf("%d argument need, but %d given\n", self->argc, argc_given);           \
        exit(-1);                                                                     \
        return;                                                                       \
    }                                                                                 \
    /* set first arg */                                                               \
    var_t result = env->data_stack.stack[env->data_stack.rbp + 2];                    \
    /* for every arg */                                                               \
    for (int i = env->data_stack.rbp + 3; i <= env->data_stack.rsp; i++) {            \
        var_t arg = env->data_stack.stack[i];                                         \
        if (type_check(arg, type)) {                                                  \
            lambda                                                                    \
        } else {                                                                      \
            /*// ! error*/                                                            \
            printf("error: type error\n");                                            \
            print_var_val(env->data_stack.stack[i]);                                  \
            fflush(stdout);                                                           \
            exit(-1);                                                                 \
            return;                                                                   \
        }                                                                             \
    }                                                                                 \
    env->result = set_var_val(type, {.type = result.type});                           \
    return;                                                                           \
}                                                                                     \
 
DEFINE_SINGLE_OPERATION_FUNC_BODY(_ADD, lisp_int32, {result.lisp_int32 += arg.lisp_int32;});


void LISP_NATIVE_FUNC_BODY_IF(func_t *self, env_t *env) {
    return;
}
void LISP_NATIVE_FUNC_BODY_DEFINE(func_t *self, env_t *env) {
    return;
}
void LISP_NATIVE_FUNC_BODY_LAMBDA(func_t *self, env_t *env) {
    return;
}
void LISP_NATIVE_FUNC_BODY_PRINT_BOOL(func_t *self, env_t *env) {
    return;
}
void LISP_NATIVE_FUNC_BODY_PRINT_NUM(func_t *self, env_t *env) {
    return;
}