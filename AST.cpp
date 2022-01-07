#include <iostream>
#include <string>
#include "AST.hpp"

AST_node *new_ast_node(std::string *name, var_t val) {
    AST_node *node = (AST_node *)malloc(sizeof(AST_node));
    node->name = name;
    node->val = val;
    return node;
}

void free_ast_node(AST_node *node) {
    if (node->next != NULL) 
        free_ast_node(node->next);
    if (node->child != NULL)
        free_ast_node(node->child);
    free(node->name);
    free(node);
    return;
}

// ? did i really need this function?
// I mean... maybe i don't need some these feature like append end of list
void add_child(AST_node *root, AST_node *child) {
    return;
}

void append_next(AST_node *root, AST_node *child) {
    return;
}

void print_node(AST_node *node) {
    if (node->name == NULL)
        printf("NULL");
    else
        printf("%s", node->name->data());
    
    printf(", ");
    printf("0x%016lx\n", node->val._content);
    fflush(stdout);
    return;
}
void visit(AST_node *root) {
    if (root == NULL)
        return;

    print_node(root);
    if (root->child != NULL) {
        printf("go child\n");
        visit(root->child);
    }

    if (root->name != NULL) {
        printf("go next\n");
        visit(root->next);
    }
    return;
}