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
    printf("EXIT\n");
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

    var_t arg1 = get_local_var(env, 2);
    bool condition = get_runtime_var_val(env, arg1, lisp_bool).lisp_bool;

    var_t choice = get_local_var(env, condition ? 3 : 4);   // choose if arg1 then arg2 else arg3
    env->result = get_runtime_var_val(env, choice, lisp_any);
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
    var_t val = get_runtime_var_val(env, arg2, lisp_any);

    var_memory_t var_mem = {.is_dynamic = true, .memory = {.dyn_var=val}};
    if (scope->id_map == NULL)
        scope->id_map = new id_map_t();

    (*scope->id_map)[*name] = var_mem;      // copy by value

#ifdef DEBUG
    printf("\nDEBUG: Mapping %s to", name->data());
    print_var_val(val);
    printf("\n");
#endif

    env->result = set_var_val(lisp_nil, {._content = 0});
}

/**
 * Structure:
 * ```
 *              arg1   arg2
 *     Lambda -> () -> () -> () -> ...
 *               |
 *               a -> b -> c
 * ```
 * Step1: set parameters
 *   for each item in arg1->child
 *      new_fun.add_param(item)
 * 
 * Step2: set body
 *                               arg2
 *  new_func.body -> push_last -> () -> () -> ...
 **/
void LISP_NATIVE_FUNC_BODY_LAMBDA(func_t *self, env_t *env) {
    int argc_given = env->data_stack.rsp - env->data_stack.rbp - 1;
    assert_argc(env, self, argc_given);

    var_t arg1 = get_local_var(env, 2);
    var_t arg2 = get_local_var(env, 3);

    func_t *scope = self->static_parent;
    func_t *new_func = new_empty_lambda(scope->scope_level + 1);

    // set parameters
    int arg_cnt = 1;
    if (type_check(arg1, lisp_ast_ptr)) {
        AST_node *name_node = get_ast_ptr(arg1)->child;
        while (name_node != NULL) {
            assert_type(env, lisp_symbol, name_node->val);
            arg_cnt++;
            (*new_func->id_map)[*(get_symbol_ptr(name_node->val))] = {.is_dynamic=false, .memory={.offset=arg_cnt}};
            name_node = name_node->next;
        }
    } else if (!type_check(arg1, lisp_nil)) {
        raise_type_error(env, (type_mask)(lisp_ast_ptr | lisp_nil), arg1);
    }
    new_func->argc = arg_cnt - 1;

    // set body
    AST_node *body_root = create_ast_nf_node(&LISP_NATIVE_FUNC_PUSH_LAST_ARG_INFO, NULL);
    AST_node *entry;

    // lambda body can be compound expression or single value
    if (type_check(arg2, lisp_ast_ptr)) {
        entry = get_ast_ptr(arg2);
    } else if (type_check(arg2, lisp_int32) || type_check(arg2, lisp_bool)) {
        entry = new_ast_node(NULL, arg2);
    } else {
        raise_type_error(env, (type_mask)(lisp_ast_ptr | lisp_int32 | lisp_bool), arg2);  // TODO: multi type
    }
    body_root->next = entry;
    new_func->body.ast_body = body_root;

    env->result = set_var_val(lisp_ptr, {.lisp_ptr = new_func});
}

void LISP_NATIVE_FUNC_BODY_PUSH_LAST_ARG(func_t *self, env_t *env) {
    env->result = env->data_stack.stack[env->data_stack.rsp];
}
