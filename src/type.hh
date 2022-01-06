#include <map>
#include <string>

typedef union LISP_VAR_TYPE{
    bool lisp_bool;
    int lisp_int;
    void *lisp_ptr;
} var_type;

struct AST_NODE;

typedef struct LISP_FUNCTION_TYPE {
    int argc;
    std::map<char*, int> id_map;    // map id name to stack offset
    struct AST_NODE *body;                 //? maybe i can use bytecode?
} func_t;

typedef struct AST_NODE {
    int node_type;                  //? maybe i don't need this property
    var_type val;
    struct AST_NODE *next;
    struct AST_NODE *child;
} AST_node;
