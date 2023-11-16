main: src/scanner.l
	mkdir -p build
	flex -o build/lex.yy.c src/scanner.l
	gcc build/lex.yy.c -o build/main

run: main
	cat tests/basics.md | ./build/main