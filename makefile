yacc     := src/yacc.y
parser_c := compiled-c/yacc.tab.c
parser_h := compiled-h/yacc.tab.h
parser_o := bin/yacc.tab.o
lex := src/lex.l
scanner_c := compiled-c/lex.yy.c
scanner_o := bin/lex.yy.o
exec := bin/nmsl

all: $(exec)

$(parser_o):
	bison -d -o $(parser_c) $(yacc)
	gcc -c -g -I .. -o $(parser_o) $(parser_c)

$(scanner_o): $(parser_o)
	flex -o $(scanner_c) $(lex)
	gcc -c -g -I .. -o $(scanner_o) $(scanner_c)

$(exec): $(scanner_o) $(parser_o)
	gcc -o $(exec) $(scanner_o) $(parser_o) -ll

clean:
	rm bin/*
	rm compiled-c/*
	rm $(exec)

test: $(exec)
	./bin/nmsl < ./test/test1.lisp