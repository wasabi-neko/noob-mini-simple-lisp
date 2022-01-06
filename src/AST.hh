#include "type.hh"

typedef struct AST_NODE {
    // int node_type;                  //? maybe i don't need this property
    var_t val;
    struct AST_NODE *next;
    struct AST_NODE *child;
} AST_node;

AST_node *new_ast_node();
void add_child(AST_node *root, AST_node *child);
void append_next(AST_node *root, AST_node *child);
