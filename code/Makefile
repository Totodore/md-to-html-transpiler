RM		= rm -f

ifdef VERBOSE
DEBUG ?= $(VERBOSE)
endif

all: compo

compo: out
	bison $(if $(VERBOSE),-tv -Wcounterexamples) -o src/y.tab.c -d src/parser.y
	flex $(if $(VERBOSE),-d) -o src/lex.yy.c src/scanner.l
	gcc $(if $(DEBUG),-g) -o out/compo src/lex.yy.c src/y.tab.c src/ast.c src/code_generation.c

out:
	mkdir -p out

.PHONY: clean re

clean:
	$(RM) out/compo out/result.html src/lex.yy.c src/y.tab.c src/y.tab.h

re: clean all
