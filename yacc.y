%{
#include <stdio.h>

void yyerror(const char *msg);
%}

%union {
    int int_val;
    int bool_val;
    char *str;
}

%token '('
%token ')'

%token INT_VAL
%token BOOL_VAL
%token ID

%token PRINT_NUM
%token PRINT_BOOL

%token '+'
%token '-'
%token '*'
%token '/'
%token MOD
%token '>'
%token '<'
%token '='
%token AND
%token OR

%token IF
%token FUN
%token DEFINE

%%
// MAIN
program : stmts
        ;
stmts   : stmt stmts
        |
        ;
stmt    : expr
        | def_stmt
        | print_stmt
        ;

// PRINT
print_stmt : '(' PRINT_NUM expr ')'
        | '(' PRINT_BOOL expr ')'
        ;

// EXP
exprs   : expr exprs
        |
        ;
expr    : INT_VAL
        | BOOL_VAL
        | var_name
        | fun_expr
        | if_expr
        | fun_call
        | '(' operator exprs ')'
        ;

if_expr : '(' IF expr expr expr ')'
        ;

operator  : '+'
          | '-'
          | '*'
          | '/'
          | MOD
          | '>'
          | '<'
          | '='
          | AND
          | OR
          ;

// Define
def_stmt : '(' DEFINE var_name expr ')'
         ;
var_name : ID
         ;

// Function
fun_expr  : '(' FUN fun_param fun_body ')'
          ;
fun_param : '(' ids ')'
          ;
ids       : ID ids
          |
          ;
fun_body  : exprs
          ;
fun_call  : '(' var_name params ')'
          | '(' fun_expr params ')'
          ;
params    : exprs
          ;

%%

void yyerror(const char *msg)
{
    printf("%s\n", msg);
    return;
}

int main(int argc, char **argv)
{
    yyparse();
    return 0;
}
