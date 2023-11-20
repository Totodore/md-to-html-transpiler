/*** Definition Section for global state ***/
%{
	int count = 0;
%}

/*** Rules Section ***/
%%

^"#" 			{ printf("%s is a h1 !\n", yytext); count++; }
^"##"    		{ printf("%s is a h2\n", yytext); }
\*\*.+\*\*		{ printf("%s is a bold text\n", yytext); }
\!\[.+\]\(.+\)	{ printf("%s is an image\n", yytext); }
\>.+			{ printf("%s is a quote\n", yytext); }
\`.+\`			{ printf("%s is an inline code\n", yytext); }
\`\`\`[^\`]+\`\`\`	{ printf("%s is a block code\n", yytext); }
\*\*\*+|\-\-\-+|\_\_\_+	{ printf("%s is a horizontal rule\n", yytext); }
(\>\.*\n)+                   { printf("Multiple and/or indented quotes\n"); }
.        		{ printf("It is not a header\n"); }

%%

/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	return 0;
}
