#include "runtime.hpp"
#include "type.hpp"

void core_dump(env_t *);
void trace_back(env_t *);
void exit_interpreter(env_t *);

void raise_type_error(env_t *, enum var_types type_expect, var_t the_var);
void raise_not_callable_error(env_t *, var_t the_var);
void raise_name_error(env_t *, std::string *symbol);
void raise_argc_error(env_t *, func_t *func, int argc_given);
void raise_stackoverflow_error(env_t*);
