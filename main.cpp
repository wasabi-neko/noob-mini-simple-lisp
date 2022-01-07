#include "type.hpp"
#include "AST.hpp"

extern "C" int yyparse(void);
extern func_t lisp_main;

int main(int argc, char *argv[]) {
    yyparse();

    visit(lisp_main.body.ast_body);
    return 0;
}