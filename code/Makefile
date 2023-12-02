RM		= rm -f

ifdef VERBOSE
DEBUG ?= $(VERBOSE)
endif

all: compo

compo: out
	flex -o src/lex.yy.c src/scanner.l
	gcc src/lex.yy.c -lfl -o out/compo

out:
	mkdir -p out

.PHONY: clean re

clean:
	$(RM) out/compo src/lex.yy.c

re: clean all
