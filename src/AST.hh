#include "type.hh"

/**
 * AST_NODE:
 * name  {char*}     the name of this ID (if this node is an ID)
 * val   {var_t}     the value of this node (if this node is not an ID)
 * next  {AST_node*} the next AST node (the basic struct of an AST is a list)
 * child {AST_node*} the child AST node (the "children" can be consider as the "arguments" of this node)
 **/
typedef struct AST_NODE {
    // int node_type;                  //? maybe i don't need this property
    char *name;
    var_t val;
    struct AST_NODE *next;
    struct AST_NODE *child;
} AST_node;

AST_node *new_ast_node(var_t val);
void free_ast_node(AST_node *node);
void add_child(AST_node *root, AST_node *child);
void append_next(AST_node *root, AST_node *child);
