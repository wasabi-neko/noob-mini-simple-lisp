#ifndef INCLUDE_NATIVE_FUNC_HPP
#define INCLUDE_NATIVE_FUNC_HPP
#include "type.hpp"
#include "AST.hpp"

#define NATIVE_FUNC_SCOPE_LEVEL 0x7fffffff

typedef void (*native_func_body_t)(func_t *self, env_t *env);

// Expands to:
//   LISP_NATIVE_FUNC_"func_id"_INFO = {
//       is_native = true;
//       name = NULL;
//       scope_level = -1;
//       argc = argc;
//       map = NULL;
//       static_parent = NULL;
//       body = natvie_func;
//  }
#define DEFINE_LISP_NATIVE_FUNC_INFO(FUNC_ID, name, allow_expr,argc)            \
    void LISP_NATIVE_FUNC_BODY##FUNC_ID(func_t *self, env_t *env);   \
    const func_t LISP_NATIVE_FUNC##FUNC_ID##_INFO = {                \
        true, allow_expr, name, NATIVE_FUNC_SCOPE_LEVEL, argc,                                        \
        NULL, NULL, -1,                                              \
        {.native_body = LISP_NATIVE_FUNC_BODY##FUNC_ID}              \
    };                                                               \

inline std::string *_lisp_native_name_string(const char *name) {
    std::string *str = new std::string("lisp_native_");
    *str += name;
    return str;
}

DEFINE_LISP_NATIVE_FUNC_INFO(_MAIN, _lisp_native_name_string("lisp_main"), false, -1)

DEFINE_LISP_NATIVE_FUNC_INFO(_ADD, _lisp_native_name_string("add"), false, -1)
DEFINE_LISP_NATIVE_FUNC_INFO(_MIN, _lisp_native_name_string("minus"), false, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_MUL, _lisp_native_name_string("multiply"), false, -1)
DEFINE_LISP_NATIVE_FUNC_INFO(_DIV, _lisp_native_name_string("divide"), false, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_MOD, _lisp_native_name_string("modulus"), false, 2)

DEFINE_LISP_NATIVE_FUNC_INFO(_GT, _lisp_native_name_string("greater-than"), false, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_LT, _lisp_native_name_string("less-than"), false, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_EQU, _lisp_native_name_string("equal"), false, -1)

DEFINE_LISP_NATIVE_FUNC_INFO(_AND, _lisp_native_name_string("and"), false, -1)
DEFINE_LISP_NATIVE_FUNC_INFO(_OR, _lisp_native_name_string("or"), false, -1)
DEFINE_LISP_NATIVE_FUNC_INFO(_NOT, _lisp_native_name_string("not"), false, 1)

DEFINE_LISP_NATIVE_FUNC_INFO(_IF, _lisp_native_name_string("if"), true, 3)
DEFINE_LISP_NATIVE_FUNC_INFO(_DEFINE, _lisp_native_name_string("define"), true, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_LAMBDA, _lisp_native_name_string("lambda"), true, -1)   // argc: at least 2
DEFINE_LISP_NATIVE_FUNC_INFO(_PRINT_BOOL, _lisp_native_name_string("print-bool"), false, 2)
DEFINE_LISP_NATIVE_FUNC_INFO(_PRINT_NUM, _lisp_native_name_string("print-num"), false, 2)

/**
 * nf stands for Native Function
 **/
AST_node *create_ast_nf_node(const func_t *native_info, func_t *parent);

#endif