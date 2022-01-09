#include "type.hpp"
#include "AST.hpp"
#include "runtime.hpp"

extern "C" int yyparse(void);
extern AST_node *lisp_root;

env_t runtime_env;

int main(int argc, char *argv[]) {
    yyparse();

    graph_AST(lisp_root);
    printf("\n\n");

    execute_main(lisp_root, &runtime_env);
    return 0;
}
