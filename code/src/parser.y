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
    "SVG"
};

const char* svg_tokens[] = {
    "Line",
    "Polyline",
    "Polygon",
    "Circle",
    "Ellipse",
    "Rect",
    "Text"
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
%token BOLD
%token H1
%token <text> TEXT

%type <dom> document block
%type <dom_list> block_list paragraph line text
%start document

%%

text:
    TEXT {
        DOM* dom = new_dom(TextElement, NULL);
        dom->text = $1;
        $$ = new_dom_list(dom);
    }
    | BOLD text BOLD {
        DOM* dom = new_dom(Bold, $2);
        $$ = new_dom_list(dom);
    };
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
    H1 TEXT {
        $$ = new_dom(Header1, NULL);
        $$->text = $2;
    }
    | paragraph {
        $$ = new_dom(Paragraph, $1);
    };
block_list:
    block BLANK_LINE block_list {
        if ($1 == NULL) {
            $$ = $3;
        } else {
            $$ = new_dom_list($1);

            $$->next = $3;
        }
    }
    | block {
        $$ = new_dom_list($1);
    };

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
