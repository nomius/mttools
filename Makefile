all: C2troff S2troff

install:
	cp C2troff /usr/bin
	cp S2troff /usr/bin

test: example
	cd example && make

C2troff:
	cc C2troff.c -o C2troff

S2troff: parser.l
	flex parser.l 
	cc lex.yy.c -o S2troff -lfl

clean:
	rm -f lex.yy.c S2troff C2troff example/create_regex.c.roff example/example.pdf

