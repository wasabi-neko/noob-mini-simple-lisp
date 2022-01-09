#include <iostream>
#include <string>
#include "AST.hpp"

AST_node *new_ast_node(std::string *name, var_t val) {
    AST_node *node = (AST_node *)malloc(sizeof(AST_node));
    node->name = name;
    node->val = val;
    node->child = NULL;
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

// (name, type , val);
void print_node(AST_node *node) {
    printf("(");
    if (node == NULL) {
        printf("node null);");
        return;
    }
    if (node->name == NULL)
        printf("_");
    else
        printf("%s", node->name->data());
    printf(", ");

    enum var_types type = get_type(node->val);
    print_type_str(type);
    printf(", ");

    print_var_val(node->val);
    printf(");");
    fflush(stdout);
    return;
}

void visit(AST_node *root) {
    print_node(root);
    if (root->child != NULL) {
        printf("go child\n");
        visit(root->child);
    }

    if (root->next != NULL) {
        printf("go next\n");
        visit(root->next);
    }
    return;
}

void graph_AST(AST_node *root) {
    if (root == NULL) {
        printf("NULL");
        return;
    }

    // print list
    AST_node *node = root;
    int child_cnt = 0;
    while (node != NULL) {
        if (node->child != NULL) {
            printf("[%d]", child_cnt);
            child_cnt++;
        }
        print_node(node);
        printf(" -> ");
        node = node->next;
    }
    printf("NULL");
    printf("\n");

    node = root;
    child_cnt = 0;
    while (node != NULL) {
        if (node->child != NULL) {
            printf("\n[%d]\n->", child_cnt);
            child_cnt++;

            graph_AST(node->child);
        }
        node = node->next;
    }

    return;
}