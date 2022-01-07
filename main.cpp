#include "type.hpp"

extern "C" int yyparse(void);

int main(int argc, char *argv[]) {
    var_t var;

    set_var_val(&var, lisp_int32, 14);
    printf("%d %d\n", type_check(var, lisp_int32), var.lisp_int32);

    yyparse();
    return 0;
}