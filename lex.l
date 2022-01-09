%option noyywrap

%{
extern "C" {
    int yylex();
}
#include "yacc.tab.hpp"

size_t line_count = 0;
size_t col_count = 0;
%}

digit [0-9]+
int ({digit}|\-{digit})
letter [a-z]+
simple_id {letter}+
complex_id {letter}+({digit}|{letter}|\-)+

%%

\(  {return yytext[0];}
\)  {return yytext[0];}
\+  {return yytext[0];}
\-  {return yytext[0];}
\*  {return yytext[0];}
\/  {return yytext[0];}
\>  {return yytext[0];}
\<  {return yytext[0];}
\=  {return yytext[0];}
mod {return MOD;}
and {return AND;}
or  {return OR;}

print-num  {return PRINT_NUM;}
print-bool {return PRINT_BOOL;}

if     {return IF;}
lambda {return LAMBDA;}
define {return DEFINE;}

#t {
    yylval.bval = true;
    return BOOL_VAL;
}

#f {
    yylval.bval = false;
    return BOOL_VAL;
}

({simple_id}|{complex_id}) {
    yylval.str = (char*)malloc(yyleng+1);
    strcpy(yylval.str, yytext);
    yylval.str[yyleng] = '\0';
    return ID;
}

{int} {
    yylval.ival = atoi(yytext);
    return INT_VAL;
}

.   {col_count++;};
\n  {line_count++;};
%%
