#include "native_func.hpp"
#include "runtime.hpp"
#include "type.hpp"
#include "error_handling.hpp"

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

#define DEFINE_SINGLE_OPERATION_FUNC_BODY(name, param_type, result_type, init_val, iter_lambda)   \
void LISP_NATIVE_FUNC_BODY##name (func_t *self, env_t *env) {                                     \
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;                               \
    /*argc check*/                                                                                \
    if (self->argc < 0) {                                                                         \
        if (argc_given < (self->argc + 1) * -1) {                                                 \
        }                                                                                         \
    } else {                                                                                      \
        if (argc_given != self->argc)                                                             \
            raise_argc_error(env, self, argc_given);                                              \
    }                                                                                             \
                                                                                                  \
    /*type check*/                                                                                \
    var_t *arg = &env->data_stack.stack[env->data_stack.rbp + 1];                                 \
    for (int i = 1; i <= argc_given; i++) {                                                       \
        if (!type_check(arg[i], param_type))                                                      \
            raise_type_error(env, param_type, arg[i]);                                            \
    }                                                                                             \
                                                                                                  \
    /* for every arg from arg[2] to end */                                                        \
    var_t result = init_val;                                                                      \
    for (int i = 2; i <= argc_given; i++) {                                                       \
        iter_lambda                                                                               \
    }                                                                                             \
    env->result = set_var_val(result_type, {.result_type = result.result_type});                  \
    return;                                                                                       \
}                                                                              
 
DEFINE_SINGLE_OPERATION_FUNC_BODY(_ADD, lisp_int32, lisp_int32, arg[1], {result.lisp_int32 += arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_MIN, lisp_int32, lisp_int32, arg[1], {result.lisp_int32 -= arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_MUL, lisp_int32, lisp_int32, arg[1], {result.lisp_int32 *= arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_DIV, lisp_int32, lisp_int32, arg[1], {result.lisp_int32 /= arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_MOD, lisp_int32, lisp_int32, arg[1], {result.lisp_int32 %= arg[i].lisp_int32;});

DEFINE_SINGLE_OPERATION_FUNC_BODY(_GT,  lisp_int32, lisp_bool, {}, {result.lisp_bool = arg[1].lisp_int32 > arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_LT,  lisp_int32, lisp_bool, {}, {result.lisp_bool = arg[1].lisp_int32 < arg[i].lisp_int32;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_EQU, lisp_int32, lisp_bool, {.lisp_bool=true}, 
                                                               {result.lisp_bool &= arg[i-1].lisp_int32 == arg[i].lisp_int32;});

DEFINE_SINGLE_OPERATION_FUNC_BODY(_AND, lisp_bool, lisp_bool, arg[1], {result.lisp_bool &= arg[i].lisp_bool;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_OR,  lisp_bool, lisp_bool, arg[1], {result.lisp_bool |= arg[i].lisp_bool;});
DEFINE_SINGLE_OPERATION_FUNC_BODY(_NOT, lisp_bool, lisp_bool, {.lisp_bool = !arg[1].lisp_bool}, {});


void LISP_NATIVE_FUNC_BODY_PRINT_BOOL(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    return;
}

void LISP_NATIVE_FUNC_BODY_PRINT_NUM(func_t *self, env_t *env) {
    return;
}

void LISP_NATIVE_FUNC_BODY_IF(func_t *self, env_t *env) {
    return;
}

void LISP_NATIVE_FUNC_BODY_DEFINE(func_t *self, env_t *env) {
    return;
}

void LISP_NATIVE_FUNC_BODY_LAMBDA(func_t *self, env_t *env) {
    return;
}
