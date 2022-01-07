#include "type.hpp"

// -----------------------------------------------------------------------------
// Variable type
// -----------------------------------------------------------------------------
var_t create_var(enum var_types type, var_t val) {
    var_t tmp;
    tmp._content = val._content | (type_mask) type;
    return tmp;
}

void set_var_val(var_t *var, enum var_types type, var_t val) {
    var->_content = val._content |  (type_mask)type;
    return;
}

bool type_check(const var_t var, enum var_types type) {
    // return true if the type tag of the var is equal to the given type
    return !((var._content & tag_mask) ^ (type_mask)type);
}

enum var_types get_type(const var_t);


// -----------------------------------------------------------------------------
// Function type
// -----------------------------------------------------------------------------

func_t *new_func(bool is_native, std::string *name, int argc, std::map<char*, int> *id_map, func_t *parent, func_body_t body) {
    func_t *func = (func_t*) malloc(sizeof(func_t));
    func->is_native = is_native;
    func->name = name;
    func->argc = argc;
    func->static_parent = parent;
    func->id_map = id_map;
    func->body = body;
    return func;
}

void free_func(func_t *);
void evoke_func(func_t *, int argc, env_t *env);