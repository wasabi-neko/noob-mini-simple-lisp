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
%token LAMBDA
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
%type <node> func_to_call
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

expr    : single_val                    {$$ = $1;}
        | var_name                      {$$ = $1;}
        | '(' func_to_call exprs ')'    {
                                            $2->next = $3;
                                            $$ = new_ast_node(NULL, set_var_val(lisp_nil, {._content = 0}));
                                            $$->child = $2;
                                        }
        | '(' ')'                       {
                                            $$ = new_ast_node(NULL, set_var_val(lisp_nil, {._content = 0}));
                                        }
        ;

single_val : INT_VAL    {$$ = new_ast_node(NULL, set_var_val(lisp_int32, {.lisp_int32 = $1}));}
           | BOOL_VAL   {$$ = new_ast_node(NULL, set_var_val(lisp_bool, {.lisp_int32 = $1}));}
           ;

var_name   : ID {
                    std::string*  id_str = new std::string($1);
                    var_t var = set_var_val(lisp_symbol, {.lisp_ptr = 0});
                    $$ = new_ast_node(id_str, var);
                }
           ;

func_to_call : keyword      {$$ = $1;}
             | buildin_func {$$ = $1;} 
             | operator     {$$ = $1;}
             | var_name     {$$ = $1;}
             | expr         {$$ = $1;}
             ;

keyword      : IF         {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_IF_INFO, NULL);}
             | DEFINE     {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_DEFINE_INFO, NULL);}
             | LAMBDA     {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_LAMBDA_INFO, NULL);}
             ;
buildin_func : PRINT_NUM  {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_PRINT_NUM_INFO, NULL);}
             | PRINT_BOOL {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_PRINT_BOOL_INFO, NULL);}
             ;

operator     : '+'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '-'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '*'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '/'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | MOD        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '>'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '<'        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | '='        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | AND        {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             | OR         {$$ = create_ast_nf_node(&LISP_NATIVE_FUNC_ADD_INFO, NULL);}
             ;

%%

void yyerror(const char *msg) {
    printf("%s\n", msg);
    return;
}
