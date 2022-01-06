%{
extern "C" {
    int yylex();
    int yyparse();
}

#include <stdio.h>
#include <string.h>
#include "type.hh"

void yyerror(const char *msg);
%}

%union {
    char *str;
}

%token '('
%token ')'

%token INT_VAL
%token BOOL_VAL
%token <str> ID

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

%type <str> func_name
%type <str> var_name

%%
program : exprs
        ;

exprs   : expr exprs
        |
        ;
expr    : single_val
        | '(' func_name exprs ')' {printf("fun: %s\n", $2);}
        ;

single_val : INT_VAL
           | BOOL_VAL
           | var_name
           ;

var_name   : ID {printf("var: %s\n", $1); $$ = $1;}
           ;

func_name : keyword     {$$ = "key";}
          | buildin_func {$$ = "buildin_func";}
          | operator {$$ = "op";}
          | var_name    {$$ = $1;}
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

void yyerror(const char *msg) {
    printf("%s\n", msg);
    return;
}

int main(int argc, char *argv[]) {
    yyparse();
    return 0;
}