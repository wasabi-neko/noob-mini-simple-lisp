#ifndef INCLUDE_NATIVE_FUNC_HPP
#define INCLUDE_NATIVE_FUNC_HPP
#include "type.hpp"
#include "AST.hpp"


typedef void (*native_func_body_t)(func_t *self, env_t *env);

// Expand:
//   LISP_NATIVE_FUNC_"func_id"_INFO
#define DEFINE_LISP_NATIVE_FUNC_INFO(FUNC_ID, name, argc)            \
    void LISP_NATIVE_FUNC_BODY##FUNC_ID(func_t *self, env_t *env);   \
    const func_t LISP_NATIVE_FUNC##FUNC_ID##_INFO = {                \
        true, name, argc,                                            \
        NULL, NULL,                                                  \
        {.native_body = LISP_NATIVE_FUNC_BODY##FUNC_ID}              \
    };                                                               \

inline std::string *_lisp_native_name_string(const char *name) {
    std::string *str = new std::string("lisp_native_");
    *str += name;
    return str;
}

DEFINE_LISP_NATIVE_FUNC_INFO(_ADD, _lisp_native_name_string("add"), -1)
// DEFINE_LISP_NATIVE_FUNC_INFO(_MIN, _lisp_native_name_string("minus"),-1)
// DEFINE_LISP_NATIVE_FUNC_INFO(_MUL, _lisp_native_name_string("multiply"),-1)
// DEFINE_LISP_NATIVE_FUNC_INFO(_DIV, _lisp_native_name_string("division"),-1)

DEFINE_LISP_NATIVE_FUNC_INFO(_IF, _lisp_native_name_string("if"), 3)
DEFINE_LISP_NATIVE_FUNC_INFO(_DEFINE, _lisp_native_name_string("define"), 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_LAMBDA, _lisp_native_name_string("lambda"), -1)   // argc: at least 2
DEFINE_LISP_NATIVE_FUNC_INFO(_PRINT_BOOL, _lisp_native_name_string("print-bool"), 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_PRINT_NUM, _lisp_native_name_string("print-num"), 2)

/**
 * nf stands for Native Function
 **/
AST_node *create_ast_nf_node(const func_t *native_info, func_t *parent);

/**
 * Hidden parameter: argc
 * example:
 *   lisp code: `(+ 1 2 a)`
 *   real call:
 *     ```
 *      push 1
 *      push 2
 *      push <val of a>     # env.getVarVal(func.id_map[a])
 *      push 3              # numbers of arguments
 *     ```
 **/

#endif