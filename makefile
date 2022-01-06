CC := g++

yacc     := src/yacc.y
parser_c := src/yacc.tab.cc
parser_h := src/yacc.tab.hh
parser_o := bin/yacc.tab.o

lex := src/lex.l
scanner_c := src/lex.yy.cc
scanner_o := bin/lex.yy.o

exec := bin/nmsl

all: $(exec)

$(parser_o):
	bison -d -o $(parser_c) $(yacc)
	$(CC) -c -g -I .. -o $(parser_o) $(parser_c)

$(scanner_o): $(parser_o)
	flex -o $(scanner_c) $(lex)
	$(CC) -c -g -I .. -o $(scanner_o) $(scanner_c)

$(exec): $(scanner_o) $(parser_o)
	$(CC) -o $(exec) $(scanner_o) $(parser_o) -ll

clean:
	-rm bin/*
	-rm $(parser_c)
	-rm $(scanner_c)
	-rm $(parser_h)
	-rm $(exec)

test: $(exec)
	./bin/nmsl < ./test/test1.lisp