#ifndef INCLUDE_RUNTIME_HH
#define INCLUDE_RUNTIME_HH

#include "type.hpp"
#include "AST.hpp"

#define RUNTIME_STACK_SIZE_MAX 10000
#define RUNTIME_FUNC_CALL_STACK_MAX 1000


/**
 * Runtime environment
 * 
 * - Stack:
 *   - rsp: stack pointer, keep point to the top element
 *   - rbp: stack base pointer, keep point to dynamic_link(the last rbp value saved in stack)
 **/
typedef struct RUNTIME_ENV {
    // register
    var_t result;

    // data stack
    struct DATA_STACK {
        var_t stack[RUNTIME_STACK_SIZE_MAX];
        int rbp;
        int rsp;
    } data_stack;

    //? maintain another funcall_stack is not very neccesary, but it's way more easier to operator another 
    //? call stack instead searching dynamic links and static links in data stack
    // function call stack
    struct FUNC_STACK {
        func_t *stack[RUNTIME_FUNC_CALL_STACK_MAX];
        int rsp;
        // rbp = 0;
    } func_stack;
} env_t;

// print methods
void dump_data_stack(env_t *env);
void dump_func_stack(env_t *env);

// Data_stack Methods
void push_stack(env_t *, var_t);
var_t pop_stack(env_t *);
var_t get_local_var(env_t *, int offset);

void create_stack_frame(env_t *);
void release_stack_frame(env_t *);

// Func_stack Methods
func_t *get_top_func(env_t *);
void add_func_call_stack(env_t *, func_t *func_template);
void pop_func_stack(env_t *);

// Runtime Methods
var_t ask_symbol(env_t *, std::string*);
var_t interpret_ast(AST_node *, env_t *, bool allow_exp_arg);
var_t execute_main(func_t *, env_t *);


#endif