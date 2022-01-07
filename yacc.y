%{
extern "C" {
    int yylex();
    int yyparse();
}

#include <stdio.h>
#include <string>

#include "AST.hpp"
#include "type.hpp"
#include "runtime.hpp"
#include "native_func.hpp"

void yyerror(const char *msg);

func_t lisp_main;
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
program : exprs         {lisp_main.body.ast_body = $1;}
        ;

exprs   : expr exprs    {$1->next = $2; $$ = $1;}
        |               {$$ = NULL;}
        ;
expr    : single_val                {$$ = $1;}
        | '(' ')'                   {$$ = new_ast_node(new std::string("nil"), create_var(lisp_nil, {._content = 0}));}
        | '(' func_name exprs ')'   {$2->child = $3; $$ = $2;}
        ;

single_val : INT_VAL    {$$ = new_ast_node(NULL, create_var(lisp_int32, {.lisp_int32 = $1}));}
           | BOOL_VAL   {$$ = new_ast_node(NULL, create_var(lisp_bool, {.lisp_int32 = $1}));}
           | var_name   {$$ = $1;}
           ;

var_name   : ID {
                    var_t var = {._content = 0};
                    $$ = new_ast_node(new std::string($1), var);
                }
           ;

func_name  : keyword      {$$ = $1;}
           | buildin_func {$$ = $1;} 
           | operator     {$$ = $1;}
           | var_name     {$$ = $1;}
           ;

keyword      : IF         
             | DEFINE
             | FUN
             ;
buildin_func : PRINT_NUM
             | PRINT_BOOL
             ;

operator     : '+'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
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
