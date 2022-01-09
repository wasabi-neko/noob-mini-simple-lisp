#ifndef INCLUDE_TYPE_HH
#define INCLUDE_TYPE_HH

#include <map>
#include <string>

// * pre-defines
// type.hh
typedef union LISP_VAR_TYPE var_t;
typedef struct LISP_FUNCTION_TYPE func_t;

// AST.hh
typedef struct AST_NODE AST_node;

// runtime.hh
typedef struct RUNTIME_ENV env_t;
typedef void (*native_func_body_t)(func_t *self, env_t *env);

// -----------------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------------
/**
 * use 64 bit data to represent all variables, useing data taggin to distinguish between types
 * |    64 bit var    |
 * |00...| 1 bit bool |
 * |01...| 32 bit int |
 * |11...| 48 bit ptr |
 */
typedef enum TYPE_MASK {
    tag_mask     = 0xff00000000000000,
    ptr48_mask   = 0x0000000000000000,
    int32_mask   = 0x1000000000000000,
    bool_mask    = 0x2000000000000000,
    nil_mask     = 0x3000000000000000,
    symbol_mask  = 0x4000000000000000,
    ast_ptr_mask = 0x5000000000000000,
} type_mask;

enum var_types {
    lisp_ptr = ptr48_mask,
    lisp_int32 = int32_mask,
    lisp_bool = bool_mask,
    lisp_nil = nil_mask,
    lisp_symbol = symbol_mask,
    lisp_ast_ptr  = ast_ptr_mask,
};
typedef union LISP_VAR_TYPE{
    int64_t _content;
    bool lisp_bool;
    int lisp_int32;
    void *lisp_ptr;
    func_t *func_ptr;
} var_t;

var_t set_var_val(enum var_types, var_t val);
bool type_check(const var_t, enum var_types);
enum var_types get_type(const var_t);
void print_type_str(enum var_types);
void print_var_val(const var_t);

// int get_int_val(const var_t);       // ? maybe i don't need this 
// bool get_bool_val(const var_t);
// func_t *get_func_ptr(const var_t);
AST_node *get_ast_ptr(const var_t);


// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------
typedef union FUNCTION_BODY_T {
        struct AST_NODE *ast_body;                 //? maybe i can use bytecode?
        native_func_body_t native_body;
} func_body_t;

typedef std::map<std::string, int> id_map_t;

typedef struct LISP_FUNCTION_TYPE {
    bool is_native;
    bool allow_exp_arg;
    std::string *name;                          // not neccesary for runtime
    int scope_level;                            // the lexical_level of the function. for determind static parent in runtime
    int argc;                                   // -1 means at least 0; -2 meas at least 1
    id_map_t *id_map;               // map id name to stack offset. (contain args_name and local variable)
    struct LISP_FUNCTION_TYPE *static_parent;
    int runtime_rbp;                            // the rbp position in it's stack frame
    func_body_t body;
} func_t;

func_t *new_func(bool is_native, std::string *name, int level, int argc, id_map_t *id_map, func_t *parent, func_body_t body);
func_t *clone_func(func_t *);
void free_func(func_t *);
void evoke_func(func_t *, int argc, env_t *env);

#endif