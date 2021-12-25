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

%token IF
%token FUN
%token DEFINE

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

%%
program : exprs
        ;

exprs   : expr exprs
        |
        ;
expr    : single_val
        | '(' func_name exprs')'
        ;

single_val : INT_VAL
           | BOOL_VAL
           | var_name
           ;

var_name   : ID
           ;

func_name : keyword
          | buildin_func
          | operator
          | var_name
          ;

keyword      : IF
             | DEFINE
             | FUN
             ;
buildin_func : PRINT_NUM
             | PRINT_BOOL
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
