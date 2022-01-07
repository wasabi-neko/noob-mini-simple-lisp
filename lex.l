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
letter [a-z]+

%%
[{digit}|\-{digit}]          {return INT_VAL;}
[#t|#f]                      {return BOOL_VAL;}

\( {return yytext[0];}
\) {return yytext[0];}
\+ {return yytext[0];}
\- {return yytext[0];}
\* {return yytext[0];}
\/ {return yytext[0];}
\> {return yytext[0];}
\< {return yytext[0];}
\= {return yytext[0];}
mod {return MOD;}
and {return AND;}
or  {return OR;}

print-num {return PRINT_NUM;}
print-bool {return PRINT_BOOL;}

if {return IF;}
lambda {return FUN;}
define {return DEFINE;}
{letter}({letter}|digit|\-)* {yylval.str = (char*)malloc(yyleng+1); strcpy(yylval.str, yytext); yylval.str[yyleng] = '\0'; return ID;}

.   {col_count++;};
\n  {line_count++;};
%%
