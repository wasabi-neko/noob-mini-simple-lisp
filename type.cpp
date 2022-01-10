#include "AST.hpp"
#include "type.hpp"

// -----------------------------------------------------------------------------
// Variable type
// -----------------------------------------------------------------------------

var_t set_var_val(enum var_types type, var_t val) {
    return {._content = (int64_t)(val._content | (type_mask)type)};
}

bool type_check(const var_t var, enum var_types type) {
    // return true if the type tag of the var is equal to the given type
    if (type == lisp_any)
        return true;
    return !((var._content & tag_mask) ^ (type_mask)type);
}

AST_node *get_ast_ptr(const var_t var) {
    return (AST_node *)(var._content ^ (type_mask)lisp_ast_ptr);
}

std::string *get_symbol_ptr(const var_t var) {
    return (std::string *)(var._content ^ (type_mask)lisp_symbol);
}

enum var_types get_type(const var_t var) {
    if (type_check(var, lisp_nil)) {
        return lisp_nil;
    } else if (type_check(var, lisp_symbol)) {
        return lisp_symbol;
    } else if (type_check(var, lisp_bool)) {
        return lisp_bool;
    } else if (type_check(var, lisp_int32)) {
        return lisp_int32;
    } else if (type_check(var, lisp_ast_ptr)) {
        return lisp_ast_ptr;
    } else if (type_check(var, lisp_ptr)) {
        return lisp_ptr;
    }
    return lisp_nil;
}

void print_type_str(enum var_types type) {
    switch (type) {
    case lisp_nil:
        printf("nil");
        break;
    case lisp_symbol:
        printf("symbol");
        break;
    case lisp_bool:
        printf("bool");
        break;
    case lisp_int32:
        printf("int32");
        break;
    case lisp_ast_ptr:
        printf("ast_ptr");
        break;
    case lisp_ptr:
        printf("ptr");
        break;
    default:
        printf("undefine");
        break;
    }
    return;
}

void print_var_val(const var_t var) {
    enum var_types type = get_type(var);
    switch (type) {
    case lisp_nil:
        printf("nil");
        break;
    case lisp_symbol:
        // printf("%s", (static_cast<std::string*>(var.lisp_ptr))->data());
        printf("sym");
        break;
    case lisp_bool:
        printf("bool %d", var.lisp_bool);
        break;
    case lisp_int32:
        printf("int %d", var.lisp_int32);
        break;
    case lisp_ast_ptr:
        printf("%p -> ", var.lisp_ptr);
        // print_node(get_ast_ptr(var));
        break;
    case lisp_ptr:
        printf("%p", var.lisp_ptr);
        if (var.func_ptr != NULL && var.func_ptr->name != NULL) {
            printf(" -> %s ", var.func_ptr->name->data());
        }
        break;
    default:
        printf("undefine");
        break;
    }
}

// -----------------------------------------------------------------------------
// Function type
// -----------------------------------------------------------------------------

func_t *new_func(bool is_native, std::string *name, int level, int argc, id_map_t *id_map, func_t *parent, func_body_t body) {
    func_t *func = (func_t*) malloc(sizeof(func_t));
    func->is_native = is_native;
    func->name = name;
    func->scope_level = level;
    func->argc = argc;
    func->static_parent = parent;
    func->id_map = id_map;
    func->body = body;
    return func;
}

func_t *new_empty_lambda(int scope_level) {
    func_t *func = (func_t*) malloc(sizeof(func_t));
    func->is_native = false;
    func->allow_exp_arg = false;
    func->name = NULL;
    func->scope_level = scope_level;
    func->id_map = new id_map_t();
    func->static_parent = NULL;
    return func;
}

func_t *clone_func(func_t *func) {
    func_t *clone = (func_t*) malloc(sizeof(func_t));
    clone->is_native = func->is_native;
    clone->allow_exp_arg = func->allow_exp_arg;
    clone->name = func->name;
    clone->scope_level = func->scope_level;
    clone->argc = func->argc;
    clone->static_parent = func->static_parent;
    clone->id_map = new id_map_t();
    if (func->id_map != NULL) {
       clone->id_map->insert(func->id_map->begin(), func->id_map->end());
    }
    clone->runtime_rbp = -1;
    clone->body = func->body;
    return clone;
}

void free_func(func_t *func) {
    free(func->id_map);
    free(func);
}

void evoke_func(func_t *, int argc, env_t *env);