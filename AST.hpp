#ifndef INCLUDE_AST_NODE_HH
#define INCLUDE_AST_NODE_HH


#include "type.hpp"

/**
 * AST_NODE:
 *   Just like lips, this AST is more like a list instead of a tree.
 *   - everyline is a list owned by its parent.
 *   - the child of a node is also a list.
 *   - the child_list of a node 'N' is consider as 'N''s arguments
 * 
 * Example:
 *   ```
 *   lisp: (+ 1 a (- 2 b) 2)
 *   
 *   AST:
 *   exp
 *    |
 *   '+' -> 1 -> a -> exp -> 2
 *                     |
 *                    '-' -> 2 -> b
 *   ```
 * 
 * Struct Member:
 * name  {char*}     the name of this ID (if this node is an ID)
 * val   {var_t}     the value of this node (if this node is not an ID)
 * next  {AST_node*} the next AST node (the basic struct of an AST is a list)
 * child {AST_node*} the child AST node (the "children" can be consider as the "arguments" of this node)
 **/
typedef struct AST_NODE {
    // int node_type;                  //? maybe i don't need this property
    std::string *name;
    var_t val;
    struct AST_NODE *next;
    struct AST_NODE *child;
} AST_node;

AST_node *new_ast_node(std::string *name, var_t val);
void free_ast_node(AST_node *node);
void add_child(AST_node *root, AST_node *child);
void append_next(AST_node *root, AST_node *child);

void print_node(AST_node *node);
void visit(AST_node *);
void graph_AST(AST_node *);

#endif