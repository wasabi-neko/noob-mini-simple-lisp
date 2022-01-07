#ifndef INCLUDE_RUNTIME_HH
#define INCLUDE_RUNTIME_HH

#include <stack>
#include "type.hpp"

#define RUNTIME_STACK_SIZE_MAX 10000

typedef struct RUNTIME_ENV {
    // register
    var_t result;

    // data stack
    var_t stack[RUNTIME_STACK_SIZE_MAX];
    int rbp;
    int rsp;

    // static function scope stack
    std::stack<func_t*> scope_stack;
} env_t;

void push_stack(env_t *, var_t *);
var_t *pop_stack(env_t *);
void prepare_func_call(env_t *);
void return_func_call(env_t *);


#endif