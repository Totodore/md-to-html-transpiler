/*** Definition Section for global state ***/
%{
	int count = 0;
%}

/*** Rules Section ***/
%%

\*\*.+\*\*					{printf("%s is a bold text\n", yytext);}
\*.+\*						{printf("%s is an underlined text\n", yytext);}
\__.+\__					{printf("%s is an underlined text\n", yytext);}
\--.+\--					{printf("%s is a strikethrough text\n", yytext);}
^#[^#]*\n 					{printf("%s is a h1 \n", yytext);}
^##[^#]*\n      			{printf("%s is a h2 \n", yytext);}
^###[^#]*\n					{printf("%s is a h3 \n", yytext);}
^####[^#]*\n     			{printf("%s is a h4 \n", yytext);}
^#####[^#]*\n 				{printf("%s is a h5 \n", yytext);}
^######[^#]*\n    			{printf("%s is a h6 \n", yytext);}

%%


/*** TBD:
 Paragraph					Davide
 Bold text (**...**)
 Italic text (*...*)
 Underlined text (__…__)
 Strikethrough text (--...--)
 Titles (# to ######)
 ***/


 
/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	printf("\n");
	return 0;
}
