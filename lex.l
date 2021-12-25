%{
#include "yacc.tab.h"
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
{letter}({letter}|digit|\-)* {return ID;}
.   ;
\n  ;
%%