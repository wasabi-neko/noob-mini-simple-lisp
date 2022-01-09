#include "type.hpp"
#include "AST.hpp"
#include "runtime.hpp"

extern "C" int yyparse(void);
extern func_t lisp_main;

env_t runtime_env;

int main(int argc, char *argv[]) {
    yyparse();

    graph_AST(lisp_main.body.ast_body);
    printf("\n\n");

    var_t result = execute_main(&lisp_main, &runtime_env);
    print_var_val(result);
    return 0;
}
