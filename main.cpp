#include "type.hpp"

extern "C" int yyparse(void);
extern func_t *lisp_main;

int main(int argc, char *argv[]) {
    yyparse();
    return 0;
}