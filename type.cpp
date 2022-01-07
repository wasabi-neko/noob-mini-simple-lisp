#include "type.hpp"

// -----------------------------------------------------------------------------
// Variable type
// -----------------------------------------------------------------------------

void set_var_val(var_t *var, enum var_types type, int64_t val) {
    var->_content = val |  (type_mask)type;
    return;
}

bool type_check(const var_t var, enum var_types type) {
    // return true if the type tag of the var is equal to the given type
    return !((var._content & tag_mask) ^ (type_mask)type);
}

enum var_types get_type(const var_t);