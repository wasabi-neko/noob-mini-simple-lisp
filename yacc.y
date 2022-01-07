%{
extern "C" {
    int yylex();
    int yyparse();
}

#include <stdio.h>
#include <string.h>
#include "AST.hpp"
#include "type.hpp"

void yyerror(const char *msg);
%}

%union {
    bool bval;
    int ival;
    char *str;
    struct AST_NODE *node;
}

%token '('
%token ')'

%token <ival> INT_VAL
%token <bval> BOOL_VAL
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

%type <node> exprs 
%type <node> expr
%type <node> single_val
%type <node> func_name
%type <node> var_name
%type <node> keyword
%type <node> buildin_func
%type <node> operator

%%
program : exprs
        ;

exprs   : expr exprs
        |
        ;
expr    : single_val
        | '(' func_name exprs ')' 
        ;

single_val : INT_VAL
           | BOOL_VAL
           | var_name
           ;

var_name   : ID 
           ;

func_name  : keyword     
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

operator     : '+'
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
    var_t var;

    set_var_val(&var, lisp_int32, 14);
    printf("%d %d\n", type_check(var, lisp_int32), var.lisp_int32);

    /* yyparse(); */
    return 0;
}