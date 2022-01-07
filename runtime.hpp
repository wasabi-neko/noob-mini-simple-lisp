#ifndef INCLUDE_RUNTIME_HH
#define INCLUDE_RUNTIME_HH

#include "type.hpp"

#define RUNTIME_STACK_SIZE_MAX 10000

typedef struct RUNTIME_ENV {
    // register
    var_t result;

    // stack
    var_t stack[RUNTIME_STACK_SIZE_MAX];
    int rbp;
    int rsp;
} env_t;

void push_stack(env_t *, var_t *);
var_t *pop_stack(env_t *);
void prepare_func_call(env_t *);
void return_func_call(env_t *);


// -----------------------------------------------------------------------------
// Native functions
// -----------------------------------------------------------------------------
typedef void (*native_func_body_t)(func_t *self, env_t *env);

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
void _lisp_natvie_add_(func_t *self, env_t *env);
const func_t list_native_add {
    .is_native = true,
    .name = "lisp_native_add",
    .argc = -1,
    .body.native_body = _lisp_natvie_add_
};

void _lisp_native_minus_(func_t *self, env_t *env);
const func_t list_native_minus {
    .is_native = true,
    .name = "lisp_native_minus",
    .argc = -1,
    .body.native_body = _lisp_native_minus_
};

#endif