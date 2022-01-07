CC := g++
CFLAGS := -Wall -O2


yacc := yacc.y
parser_c := yacc.tab.cpp
parser_h := yacc.tab.hpp

lex := lex.l
scanner_c := lex.yy.cpp

EXEC := bin/nmsl


all: $(EXEC)

$(parser_c):
	bison -d -o $(parser_c) $(yacc)

$(scanner_c):
	flex -o $(scanner_c) $(lex)

$(EXEC): $(scanner_c) $(parser_c)
	$(CC) $(CFLAGS) ./*.cpp -o $(EXEC)

clean:
	-rm bin/*
	-rm *.o
	-rm $(parser_c)
	-rm $(scanner_c)
	-rm $(parser_h)
	-rm $(EXEC)

test: $(EXEC)
	./bin/nmsl < ./test/test1.lisp