#ifndef INCLUDE_TYPE_HH
#define INCLUDE_TYPE_HH

#include <map>
#include <string>

// type.hh
typedef union LISP_VAR_TYPE var_t;
typedef struct LISP_FUNCTION_TYPE func_t;
// AST.hh
typedef struct AST_NODE AST_node;
// runtime.hh
typedef struct RUNTIME_ENV env_t;
typedef void (*native_func_body_t)(func_t *self, env_t *env);

/**
 * use 64 bit data to represent all variables, useing data taggin to distinguish between types
 * |    64 bit var    |
 * |00...| 1 bit bool |
 * |01...| 32 bit int |
 * |11...| 48 bit ptr |
 */
typedef enum TYPE_MASK {
    tag_mask   = 0xff00000000000000,
    bool_mask  = 0x4000000000000000,
    int32_mask = 0x8000000000000000,
    ptr48_mask = 0x0000000000000000,
} type_mask;

enum var_types {
    list_bool = bool_mask,
    lisp_int32 = int32_mask,
    list_ptr = ptr48_mask,
};
typedef union LISP_VAR_TYPE{
    int64_t _content;
    bool lisp_bool;
    int lisp_int32;
    void *lisp_ptr;
} var_t;

void set_var_val(var_t *, enum var_types, int64_t val);
enum var_types get_type(const var_t, enum var_types);
bool type_check(const var_t, enum var_types);

int get_int_val(const var_t);       // ? maybe i don't need this 
bool get_bool_val(const var_t);
func_t *get_func_ptr(const var_t);


typedef struct LISP_FUNCTION_TYPE {
    bool is_native;
    char *name;                             // not neccesary for runtime
    int argc;                               // -1 if not limited
    std::map<char*, int> id_map;    // map id name to stack offset. (contain args_name and local variable)
    struct LISP_FUNCTION_TYPE *static_parent;

    union {
        struct AST_NODE *ast_body;                 //? maybe i can use bytecode?
        native_func_body_t native_body;
    } body;
} func_t;

func_t *new_func(char *name);
void free_func(func_t *);
void evoke_func(func_t *, int argc, env_t *env);

#endif