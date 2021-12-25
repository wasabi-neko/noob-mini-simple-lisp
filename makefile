YACC := yacc.tab.o
LEX	:= lex.yy.o
EXEC := nmsl

all: $(EXEC)

$(YACC):
	bison -d -o yacc.tab.c yacc.y
	gcc -c -g -I .. yacc.tab.c

$(LEX): $(YACC)
	flex -o lex.yy.c lex.l
	gcc -c -g -I .. lex.yy.c

$(EXEC): $(LEX) $(YACC)
	gcc -o nmsl yacc.tab.o lex.yy.o -ll