#include "native_func.hpp"

AST_node *create_ast_nf_node(const func_t *native_info, func_t *parent) {
    var_t fun_ptr;
    func_t *native_clone = new_func(true, 
                                    native_info->name,
                                    native_info->argc,
                                    NULL,
                                    parent,
                                    native_info->body);
    set_var_val(&fun_ptr, lisp_ptr, {.lisp_ptr = native_clone});
    AST_node *node = new_ast_node(native_info->name, fun_ptr);
    return node;
}
 
void LISP_NATIVE_FUNC_BODY_ADD(func_t *self, env_t *env) {
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
void LISP_NATIVE_FUNC_BODY_PRINT_BOOL(func_t *self, env_t *env) {
    return;
}
void LISP_NATIVE_FUNC_BODY_PRINT_NUM(func_t *self, env_t *env) {
    return;
}