%{
/*********** C CODE (YOU DO NOT HAVE TO MODIFY IT) ******************/
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

#define CHECK_YYNOMEM(ptr) if ((ptr) == NULL) YYNOMEM;
#define CHECK_LENGTH(var, num) if ((num) >= 0) var = num; else { yyerror("negative length"); YYERROR; }

int yylex(void);
void yy_scan_string(char* s);
int yylex_destroy(void);
void yyerror(const char*);

extern char* strbuf;

const char *tokens[] = {
    "Document",
    "TextElement",
    "Paragraph",
    "Bold",
    "Italic",
    "Underline",
    "Strikethrough",
    "Header1",
    "Header2",
    "Header3",
    "Header4",
    "Header5",
    "Header6",
    "Quote",
    "Link",
    "Image",
    "InlineCode",
    "BlockCode",
    "HRule",
    "SVG",
};

const char* svg_tokens[] = {
    "Line",
    "Polyline",
    "Polygon",
    "Circle",
    "Ellipse",
    "Rect",
    "Text",
};

DOM* dom_root = NULL;

/*********** END OF C CODE ******************/
/** YOU WILL HAVE TO COMPLETE THE DOCUMENT BELOW **/
%}

%define parse.lac full
%define parse.error detailed

%union {
    char* text;
    int number;
    DOM* dom;
    DomList* dom_list;
    SvgCoord* svg_coord;
    SvgCoordList* svg_coord_list;
    SvgInst* svg;
    SvgList* svg_list;
}

%destructor { free($$); } <text>
%destructor { free_dom($$); } <dom>
%destructor { free_dom_list($$); } <dom_list>
%destructor { free($$); } <svg_coord>
%destructor { free_svg_coord_list($$); } <svg_coord_list>
%destructor { free_svg_inst($$); } <svg>
%destructor { free_svg_list($$); } <svg_list>


%token NEWLINE BLANK_LINE
%token BOLD ITALIC UNDERLINE STRIKETHROUGH
%token H1 H2 H3 H4 H5 H6 HR
%token <text> TEXT XSVG_ATTR
%token <number> NUMBER
%token LPAREN RPAREN LBRACKET RBRACKET EXCLAM_LBRACKET
%token QUOTE
%token BLOCK_CODE INLINE_CODE
%token XSVG_BEGIN XSVG_END COMMA
%token LINE POLYLINE POLYGON CIRCLE ELLIPSE RECT XSVG_TEXT

%type <dom> document block
%type <dom_list> block_list paragraph line text
%type <svg_list> svg_list
%type <svg_coord_list> svg_coord_list
%type <svg_coord> svg_coord
%type <svg> svg
%start document

%%

text:
    TEXT {
        DOM* dom = new_dom(TextElement, NULL);
        dom->text = $1;
        $$ = new_dom_list(dom);
    };
    | BOLD text BOLD {
        DOM* dom = new_dom(Bold, $2);
        $$ = new_dom_list(dom);
    };
    | ITALIC text ITALIC {
        DOM* dom = new_dom(Italic, $2);
        $$ = new_dom_list(dom);
    };
    | UNDERLINE text UNDERLINE {
        DOM* dom = new_dom(Underline, $2);
        $$ = new_dom_list(dom);
    };
    | STRIKETHROUGH text STRIKETHROUGH {
        DOM* dom = new_dom(Strikethrough, $2);
        $$ = new_dom_list(dom);
    };
	| LBRACKET TEXT RBRACKET LPAREN TEXT RPAREN {
		DOM* dom = new_dom(Link, NULL);
		dom->text = $2;
		dom->url = $5;
		$$ = new_dom_list(dom);
	};
	| EXCLAM_LBRACKET TEXT RBRACKET LPAREN TEXT RPAREN {
		DOM* dom = new_dom(Image, NULL);
		dom->text = $2;
		dom->url = $5;
		$$ = new_dom_list(dom);
	}
	| INLINE_CODE TEXT INLINE_CODE {
		DOM* dom = new_dom(InlineCode, NULL);
		dom->text = $2;
		$$ = new_dom_list(dom);
	}
svg:
	// match SVG instructions (Line..., ...)
	LINE svg_coord svg_coord XSVG_ATTR {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		coord_list->next = new_svg_coord_list($3);

		SvgInst* line = new_svg_inst(Line, coord_list);
		line->color_fill = $4;

		$$ = line;
	}
	| POLYLINE svg_coord_list XSVG_ATTR {
		SvgInst* polyline = new_svg_inst(Polyline, $2);
		polyline->color_fill = $3;

		$$ = polyline;
	}
	| POLYGON svg_coord_list XSVG_ATTR XSVG_ATTR {
		SvgInst* polygon = new_svg_inst(Polygon, $2);
		polygon->color_stroke = $3;
		polygon->color_fill = $4;

		$$ = polygon;
	}
	| CIRCLE svg_coord NUMBER XSVG_ATTR XSVG_ATTR {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		SvgInst* circle = new_svg_inst(Circle, coord_list);
		circle->width = $3;
		
		circle->color_stroke = $4;
		circle->color_fill = $5;

		$$ = circle;
	}
	| ELLIPSE svg_coord NUMBER NUMBER XSVG_ATTR XSVG_ATTR {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		SvgInst* ellipse = new_svg_inst(Ellipse, coord_list);

		ellipse->width = $3;
		ellipse->height = $4;

		ellipse->color_stroke = $5;
		ellipse->color_fill = $6;

		$$ = ellipse;
	}
	| RECT svg_coord NUMBER NUMBER XSVG_ATTR XSVG_ATTR {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		SvgInst* rect = new_svg_inst(Rect, coord_list);
		rect->width = $3;
		rect->height = $4;

		rect->color_stroke = $5;
		rect->color_fill = $6;

		$$ = rect;
	}
	| XSVG_TEXT svg_coord XSVG_ATTR XSVG_ATTR XSVG_ATTR {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		SvgInst* text = new_svg_inst(Text, coord_list);
		text->text = $3;

		text->anchor = $4;
		text->color_fill = $5;

		$$ = text;
	}
svg_list:
	svg {
		$$ = new_svg_list($1);
	}
	| svg NEWLINE XSVG_END {
		$$ = new_svg_list($1);
	}
	| svg NEWLINE svg_list {
		if ($1 == NULL) {
            $$ = $3;
        } else {
            $$ = new_svg_list($1);

            $$->next = $3;
        }
	}
	| NEWLINE svg_list {
		$$ = $2;
	}
	| svg_list NEWLINE {
		$$ = $1;
	}
	| NEWLINE {
		$$ = NULL;
	}
svg_coord:
	NUMBER COMMA NUMBER {
		$$ = new_svg_coord($1, $3);
	}
svg_coord_list:
	svg_coord {
		$$ = new_svg_coord_list($1);
	}
	| svg_coord_list svg_coord {
		SvgCoordList* curr = $1;
		while (curr->next != NULL) curr = curr->next;
		curr->next = new_svg_coord_list($2);
		$$ = $1;
	}
line:
    text line {
        $$ = $1;
        $$->next = $2;
    }
    | text { $$ = $1; };
paragraph:
    line NEWLINE paragraph {
        $$ = $1;
        DomList* curr = $$;
        while (curr->next != NULL) curr = curr->next;
        curr->next = $3;
    }
    | line { $$ = $1; };
block:
    H1 text {
        $$ = new_dom(Header1, $2);
    }
    | H2 text {
        $$ = new_dom(Header2, $2);
    }
    | H3 text {
        $$ = new_dom(Header3, $2);
    }
    | H4 text {
        $$ = new_dom(Header4, $2);
    }
    | H5 text {
        $$ = new_dom(Header5, $2);
    }
    | H6 text {
        $$ = new_dom(Header6, $2);
    }
	| QUOTE text {
		$$ = new_dom(Quote, $2);
	}
    | paragraph {
        $$ = new_dom(Paragraph, $1);
    }
block_list:
	XSVG_BEGIN svg_coord COMMA svg_coord svg_list XSVG_END block_list {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		coord_list->next = new_svg_coord_list($4);
		SvgInst* coord = new_svg_inst(Coords, coord_list);

		DOM* dom = new_dom(SVG, NULL);
		dom->svg_children = $5;
		dom->svg_coords = coord_list;

		$$ = new_dom_list(dom);
		$$->next = $7;
	}
	| XSVG_BEGIN svg_coord COMMA svg_coord svg_list XSVG_END {
		SvgCoordList* coord_list = new_svg_coord_list($2);
		coord_list->next = new_svg_coord_list($4);
		SvgInst* coord = new_svg_inst(Coords, coord_list);

		DOM* dom = new_dom(SVG, NULL);
		dom->svg_children = $5;
		dom->svg_coords = coord_list;

		$$ = new_dom_list(dom);
	}
    | block BLANK_LINE block_list {
        if ($1 == NULL) {
            $$ = $3;
        } else {
            $$ = new_dom_list($1);

            $$->next = $3;
        }
    }
	| block NEWLINE block_list {
		if ($1 == NULL) {
			$$ = $3;
		} else {
			$$ = new_dom_list($1);

			$$->next = $3;
		}
	}
    | block {
        $$ = new_dom_list($1);
    }
	| NEWLINE block_list {
		$$ = $2;
	}
	| BLANK_LINE block_list {
		$$ = $2;
	}
	| HR NEWLINE block_list {
		DOM* dom = new_dom(HRule, NULL);
		$$ = new_dom_list(dom);
		$$->next = $3;
	}
	| BLOCK_CODE TEXT BLANK_LINE block_list {
		DOM* dom = new_dom(BlockCode, NULL);
		dom->text = $2;
		$$ = new_dom_list(dom);
		$$->next = $4;
	}
	| BLOCK_CODE TEXT {
		DOM* dom = new_dom(BlockCode, NULL);
		dom->text = $2;
		$$ = new_dom_list(dom);
	}
	

document: block_list {
    dom_root = $$ = new_dom(Document, $1);
    YYACCEPT;
};

%%

/*********** C CODE (YOU DO NOT HAVE TO MODIFY IT) ******************/

void svg_display(SvgInst* svg, int depth) {
    if (svg == NULL) return;
    for (int i = 0; i < depth - 2; i++) {
        printf("│   ");
    }
    printf("├─── %s", svg_tokens[svg->kind]);
    SvgCoordList* curr_coord = svg->coords;
    printf(" [");
    while (curr_coord != NULL) {
        printf(" (%d, %d)", curr_coord->coord->x, curr_coord->coord->y);
        curr_coord = curr_coord->next;
    }
    printf("]");
    printf(" w=%d", svg->width);
    printf(" h=%d", svg->height);
    if (svg->text != NULL) printf(" \"%s\"", svg->text);
    if (svg->anchor != NULL) printf(" anchor=%s", svg->anchor);
    if (svg->color_stroke != NULL) printf(" cs=%s", svg->color_stroke);
    if (svg->color_fill != NULL) printf(" cf=%s", svg->color_fill);
    printf("\n");
}

void dom_display(DOM* dom, int depth) {
    if (dom == NULL) return;
    int i = 0;

    if (depth == 1) {
        printf("%s", tokens[dom->dom_el]);
    } else {
        for (i = 0; i < depth - 2; i++) {
            printf("│   ");
        }
        printf("├─── %s", tokens[dom->dom_el]);
    }
    if (dom->text != NULL) {
        printf(" (%s)", dom->text);
    }
    if (dom->url != NULL) {
        printf(" (%s)", dom->url);
    }
	if (dom->svg_coords != NULL) {
		printf(" [");
		SvgCoordList* curr = dom->svg_coords;
		while (curr != NULL) {
			printf("(%d, %d) ", curr->coord->x, curr->coord->y);
			curr = curr->next;
		}
		printf("]");
	}
    printf("\n");

    SvgList* svg_child = dom->svg_children;

    while (svg_child != NULL) {
        svg_display(svg_child->svg, depth + 1);

        svg_child = svg_child->next;
    }

    DomList* current_child = dom->children;

    while (current_child != NULL) {
        dom_display(current_child->dom, depth + 1);

        current_child = current_child->next;
    }
}

void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}

int yywrap(void) {
    return 1;
}

int main(int argc, char* argv[]) {
#if YYDEBUG
    extern int yydebug;
    yydebug = 1;
#endif

    char* input = NULL;
    if (argc == 2) {
        FILE* f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "Cannot open file");
            return -1;
        }
        fseek(f, 0, SEEK_END);
        int length = ftell(f);
        rewind(f);
        input = malloc(sizeof(char) * (length + 1));
        if (input == NULL) {
            fprintf(stderr, "Cannot allocate enough memory for file");
            return -2;
        }
        fread(input, sizeof(char), length, f);
        fclose(f);
        *(input + length) = 0;
        unsigned int i = 0;
        while (i < length && isspace(*input)) {
            input++;
            i++;
        }
        unsigned int j = length - i - 1;
        while (j > i && isspace(*(input + j))) *(input + j--) = 0;
        yy_scan_string(input);
    }
    int ret = yyparse();

    if (input != NULL) {
        free(input);
    }
    if (argc == 2) {
        yylex_destroy();
    }
    if (strbuf != NULL) {
        free(strbuf);
    }

    if (ret > 0) return ret;
    else {
        dom_display(dom_root, 1);

        free_dom(dom_root);
    }
}

/*********** END OF C CODE ******************/
