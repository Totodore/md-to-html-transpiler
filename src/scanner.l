%{
/*********** C CODE (YOU DO NOT HAVE TO MODIFY IT) ******************/
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "y.tab.h"

/* BEGIN MODE SYSTEM DECLARATION */
int mode_caller[8];
int mode_caller_len = 0;

void mode_caller_push(int mode);
int mode_caller_peek(void);
int mode_caller_pop(void);

#define BEGIN_MODE(mode) mode_caller_push(YY_START); BEGIN((mode));
#define END_MODE BEGIN(mode_caller_pop());
/* END MODE SYSTEM DECLARATION */

/* BEGIN STRING MANIPULATION DECLARATION */
#define STRBUFCAP 1024

char* strbuf = NULL;
size_t strbuflen = 0;

void strbufapp(char c);
char* strbufclr(void);
char* str_copy(char *str);
/* END STRING MANIPULATION DECLARATION */

void print_token(char *token);
void print_token_with_str(char *token, char *str);
void print_token_with_number(char *token, int number);

/*********** END OF C CODE ******************/

/*
Here is my personal list of tokens for my grammar.
You are welcome to use it if your grammar does not match what was expected:

NEWLINE
BLANK_LINE
BOLD
ITALIC
UNDERLINE
STRIKETHROUGH
H1
H2
H3
H4
H5
H6
QUOTE
EXCLAM
LPAREN
RPAREN
LBRACKET
RBRACKET
HRULE
INLINE_CODE
BLOCK_CODE
XSVG_BEGIN
XSVG_END
COMMA
LINE
POLYLINE
POLYGON
CIRCLE
ELLIPSE
RECT
XSVG_TEXT
TEXT
STR
NUMBER
*/

/** YOU WILL HAVE TO COMPLETE THE DOCUMENT BELOW **/
%}

newline "\n"
bold "**"
h1 "#"[[:blank:]]*
h2 "##"[[:blank:]]*
h3 "###"[[:blank:]]*
h4 "####"[[:blank:]]*
h5 "#####"[[:blank:]]*
h6 "######"[[:blank:]]*
italic "*"
underline "__"
strikethrough "~~"
quote ">"
escapable_char "\\"
list_item "[[:blank:]]*\\*"
link "(\<.+\>)|(\[.+\]\(.+\))"
image "\!\[.+\]\(.+\)"
hrule "---"[-]*
inline_code \`
block_code "\`\`\`"
line "line"
polyline "polyline"
polygon "polygon"
circle "circle"
ellipse "ellipse"
rect "rect"
str "[^[:blank:]]+"
number [0-9]+
exclam "!"
lparen "("
rparen ")"
lbracket "["
rbracket "]"
blank_line [[:blank:]]*{newline}[[:blank:]]*{newline}

xsvg_begin "\`\`\`xsvg:"
svg_coord {number},{number}
svg_comma ,
xsvg_text "text"
xsvg_attribute (([a-z]+)|("#"[0-9a-fA-F]{6}))|\"([^\"]*)\"
xsvg_end "\`\`\`"

%x TEXT_MODE UNDERLINE_MODE STRIKETHROUGH_MODE ITALIC_MODE BLOCK_CODE_MODE INLINE_CODE_MODE BOLD_MODE IMAGE_MODE LINK_MODE
%x SVG_MODE  SVG_COORD_MODE
%%

<INITIAL>[[:blank:]]*{newline} {
    return NEWLINE;
};

([[:blank:]]*{newline}){2} {
    return BLANK_LINE;
};

^{h1} {
    return H1;
}

^{h2} {
    return H2;
}

^{h3} {
    return H3;
}

^{h4} {
    return H4;
}

^{h5} {
    return H5;
}

^{h6} {
    return H6;
}

^{hrule} {
    return HRULE;
};

<INITIAL>{italic} {
    BEGIN_MODE(ITALIC_MODE);
    return ITALIC;
};

<ITALIC_MODE>{italic} {
    END_MODE;
    return ITALIC;
};

<INITIAL,ITALIC_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    strbufapp(*yytext);
};

<INITIAL>{bold}	{
    BEGIN_MODE(BOLD_MODE); // You can switch to another mode

    return BOLD;
};

<BOLD_MODE>{bold} {
    END_MODE; // It will go back to the previous mode

    return BOLD;
};

<INITIAL,BOLD_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    // yytext is containing the text captured by the regular
    // expression.
    // The expression "*yytext" is to catch only one character (the first one).
    // We will append our character in a global string
    strbufapp(*yytext);
};

<INITIAL>{underline} {
    BEGIN_MODE(UNDERLINE_MODE);
    return UNDERLINE;
};

<UNDERLINE_MODE>{underline} {
    END_MODE;

    return UNDERLINE;
};

<INITIAL,UNDERLINE_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    strbufapp(*yytext);
};

<INITIAL>{strikethrough} {
    BEGIN_MODE(STRIKETHROUGH_MODE);
    return STRIKETHROUGH;
};

<STRIKETHROUGH_MODE>{strikethrough} {
    END_MODE;
    return STRIKETHROUGH;
};

<INITIAL,STRIKETHROUGH_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    strbufapp(*yytext);
};

<INITIAL>{quote}[[:blank:]]* {
	return QUOTE;
};

<INITIAL>{list_item} {
};

<INITIAL>{inline_code}. {
    BEGIN_MODE(INLINE_CODE_MODE);
	strbufapp(*(yytext + 1)); // buf again the char captured by the . (dot)
	return INLINE_CODE;
};

<INLINE_CODE_MODE>{inline_code} {
    END_MODE;

	return INLINE_CODE;
};

<INITIAL,INLINE_CODE_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    strbufapp(*yytext);
};

<INITIAL>{block_code}{newline} {
    BEGIN_MODE(BLOCK_CODE_MODE);
	return BLOCK_CODE;
};

<BLOCK_CODE_MODE>. {
	strbufapp(*yytext);
};

<BLOCK_CODE_MODE>{newline}{block_code} {
	END_MODE;

    char* txt = strbufclr(); // We retrieve the global string made with strbufapp()
    char *copy = str_copy(txt); // We have to make a copy of the global string or the yytext when needed
    yylval.text = copy;

	return TEXT;
};

<BLOCK_CODE_MODE>{newline} {
	strbufapp(*yytext);
};

<BLOCK_CODE_MODE><<EOF>> {
	END_MODE;

    char* txt = strbufclr(); // We retrieve the global string made with strbufapp()
    char *copy = str_copy(txt); // We have to make a copy of the global string or the yytext when needed
    yylval.text = copy;

	return TEXT;
};

<INITIAL,TEXT_MODE>{exclam}{lbracket} {
    BEGIN_MODE(IMAGE_MODE); // You can switch to another mode

	return EXCLAM_LBRACKET;
};

<INITIAL,TEXT_MODE>{lbracket}. {
    BEGIN_MODE(LINK_MODE); // You can switch to another mode

    strbufapp(*(yytext + 1)); // buf again the char captured by the . (dot)
	return LBRACKET;
};

<LINK_MODE,IMAGE_MODE>{rbracket} {
	return RBRACKET;
}
<LINK_MODE,IMAGE_MODE>{lparen} {
	return LPAREN;
};

<LINK_MODE,IMAGE_MODE>{rparen} {
    END_MODE; // It will go back to the previous mode
	return RPAREN;
};

<INITIAL,LINK_MODE,IMAGE_MODE>. {
    BEGIN_MODE(TEXT_MODE);

    strbufapp(*yytext);
};


<TEXT_MODE>({newline}|{inline_code}|{bold}|^{h1}|^{h2}|^{h3}|^{h4}|^{h5}|^{h6}|^{hrule}|{italic}|{underline}|{strikethrough}|{list_item}|{link}|{image}|{block_code}|{lparen}|{rparen}|{rbracket}|{lbracket}|{xsvg_begin}|{xsvg_end}|{exclam}{lbracket})(.|\n)* {
    END_MODE;

    // We give the full capture to Flex to be analyzed again
    // otherwise the content will be lost
    yy_scan_string(yytext);

    char* txt = strbufclr(); // We retrieve the global string made with strbufapp()
    char *copy = str_copy(txt); // We have to make a copy of the global string or the yytext when needed
    yylval.text = copy;
    return TEXT;
};

<TEXT_MODE><<EOF>> {
    // <<EOF>> means the end of the markdown file.
    END_MODE;

    char* txt = strbufclr();
    char *copy = str_copy(txt);

    yylval.text = copy;
    return TEXT;
};

<TEXT_MODE>. {
    strbufapp(*yytext);
};

<INITIAL>^{xsvg_begin} {
    BEGIN_MODE(SVG_MODE);

	return XSVG_BEGIN;
}

<SVG_MODE>{number} {
	yylval.number = atoi(yytext);
	return NUMBER;
}

<SVG_MODE>{svg_comma} {
	return COMMA;
}

<SVG_MODE>^"line " {
	return LINE;
}
<SVG_MODE>^"polyline " {
	return POLYLINE;
}
<SVG_MODE>^"polygon " {
	return POLYGON;
}
<SVG_MODE>^"circle " {
	return CIRCLE;
}
<SVG_MODE>^"ellipse " {
	return ELLIPSE;
}
<SVG_MODE>^"rect " {
	return RECT;
}
<SVG_MODE>^"text " {
	return XSVG_TEXT;
}
<SVG_MODE>{xsvg_end} {
    END_MODE; // It will go back to the previous mode

	return XSVG_END;
};
<SVG_MODE>{xsvg_attribute} {
	yylval.text = str_copy(yytext);
	return XSVG_ATTR;
}
<SVG_MODE>[[:blank:]]*{newline} {
    return NEWLINE;
};
%%

/*********** C CODE (YOU DO NOT HAVE TO MODIFY IT) ******************/
void mode_caller_push(int mode) {
    mode_caller[mode_caller_len++] = mode;
}

int mode_caller_peek(void) {
    return mode_caller_len == 0 ? INITIAL : mode_caller[mode_caller_len - 1];
}

int mode_caller_pop(void) {
    int caller = mode_caller_peek();
    if (mode_caller_len > 0) {
        mode_caller_len--;
    }
    return caller;
}

void strbufapp(char c) {
    if (strbuf == NULL) {
        strbuf = malloc(sizeof(char) * STRBUFCAP);
    }
    if (strbuflen >= STRBUFCAP - 2) {
        char* new_strbuf = malloc(sizeof(char) * 2 * STRBUFCAP);
        strncpy(new_strbuf, strbuf, strbuflen);
        free(strbuf);
        strbuf = new_strbuf;
    }
    strbuf[strbuflen++] = c;
}

char* strbufclr(void) {
    strbuf[strbuflen] = '\0';
    strbuflen = 0;
    return strbuf;
}

char* str_copy(char *str) {
    char* copy = malloc(strlen(str) + 1);

    strcpy(copy, str);

    return copy;
}

void print_token(char *token) {
    printf("TOKEN: %s\n", token);
}

void print_token_with_str(char *token, char *str) {
    printf("TOKEN: %s | String: \"%s\"\n", token, str);
}

void print_token_with_number(char *token, int number) {
    printf("TOKEN: %s | Number: \"%d\"\n", token, number);
}

/*********** END OF C CODE ******************/
