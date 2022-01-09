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
    printf("ERROR: the lisp_main_scope function should only used as scope instead of being evoked\n");
    exit(-1);
}

void LISP_NATIVE_FUNC_BODY_MAIN_EXIT(func_t *self, env_t *env) {
    printf("EXIT");
    exit(0);
}

#define DEFINE_SINGLE_OPERATION_FUNC_BODY(name, param_type, result_type, init_val, iter_lambda)   \
void LISP_NATIVE_FUNC_BODY##name (func_t *self, env_t *env) {                                     \
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;                               \
    /*argc check*/                                                                                \
    assert_argc(env, self, argc_given);                                                           \
    /*type check*/                                                                                \
    var_t *arg = &env->data_stack.stack[env->data_stack.rbp + 1];                                 \
    for (int i = 1; i <= argc_given; i++) {                                                       \
        assert_type(env, param_type, arg[i]);                                                     \
    }                                                                                             \
                                                                                                  \
    /* for every arg from arg[2] to end */                                                        \
    var_t result = init_val;                                                                      \
    for (int i = 2; i <= argc_given; i++) {                                                       \
        iter_lambda                                                                               \
    }                                                                                             \
    env->result = set_var_val(result_type, {.result_type = result.result_type});                  \
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
    assert_argc(env, self, argc_given);
    var_t var = get_local_var(env, 2);
    assert_type(env, lisp_bool, var);
    if (var.lisp_bool)
        printf("#t\n");
    else
        printf("#f\n");

    env->result = set_var_val(lisp_nil, {._content = 0});       // return nil
}

void LISP_NATIVE_FUNC_BODY_PRINT_NUM(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    assert_argc(env, self, argc_given);
    var_t var = get_local_var(env, 2);
    assert_type(env, lisp_int32, var);
    printf("%d\n", var.lisp_int32);

    env->result = set_var_val(lisp_nil, {._content = 0});       // return nil
}

void LISP_NATIVE_FUNC_BODY_IF(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    assert_argc(env, self, argc_given);

    bool condition;
    var_t arg1 = get_local_var(env, 2);
    if (type_check(arg1, lisp_ast_ptr)) {
        interpret_ast(get_ast_ptr(arg1)->child, env, false);
        assert_type(env, lisp_bool, env->result);
        condition = env->result.lisp_bool;
    } else {
        assert_type(env, lisp_bool, arg1);
        condition = arg1.lisp_bool;
    }

    var_t choice = get_local_var(env, condition ? 3 : 4);   // chose if arg1 then arg2 else arg3

    if (type_check(choice, lisp_ast_ptr)) {
        interpret_ast(get_ast_ptr(choice)->child, env, false);
    } else {
        env->result = choice;
    }
}

void LISP_NATIVE_FUNC_BODY_DEFINE(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    assert_argc(env, self, argc_given);
    func_t *scope = self->static_parent;

    if (scope == NULL) {
        printf("Error: not static parent!\n");
        exit(-1);
    }

    var_t arg1 = get_local_var(env, 2);
    assert_type(env, lisp_symbol, arg1);
    std::string *name = get_symbol_ptr(arg1);

    var_t arg2 = get_local_var(env, 3);
    var_t val;
    if (type_check(arg2, lisp_ast_ptr)) {
        interpret_ast(get_ast_ptr(arg2)->child, env, false);
        val = env->result;
    } else {
        val = arg2;
    }

    var_memory_t var_mem = {.is_dynamic = true, .memory = {.dyn_var=val}};
    if (scope->id_map == NULL)
        scope->id_map = new id_map_t();

    (*scope->id_map)[*name] = var_mem;      // copy by value

    printf("\nDEBUG: Mapping %s to", name->data());
    print_var_val(val);
    printf("\n");

    env->result = set_var_val(lisp_nil, {._content = 0});
}

void LISP_NATIVE_FUNC_BODY_LAMBDA(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    assert_argc(env, self, argc_given);

    env->result = set_var_val(lisp_nil, {._content = 0});
}

void LISP_NATIVE_FUNC_BODY_PUSH_LAST_ARG(func_t *self, env_t *env) {
    env->result = env->data_stack.stack[env->data_stack.rsp];
}
