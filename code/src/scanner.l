/*** Definition Section for global state ***/
%{
	int count = 0;
	int listIndex = 0;
%}

/*** Rules Section ***/
%%
^"#" 							{printf("`%s` is a h1 !\n", yytext); count++;}
^"##"     						{printf("`%s` is a h2\n", yytext);}
\*\*.+\*\*						{printf("`%s` is a bold text\n", yytext);}

\n 								{printf("New line\n");}
(\*\*\*.+\*\*\*)|(___.+___)		{printf("`%s` is a bold italic text\n", yytext);}
^"* ".+							{printf("`%s` is a list element\n", yytext);}
^"- ".+							{printf("`%s` is a list element\n", yytext);}
^[1-9][0-9]*". ".+				{printf("`%s` is an ordered list element\n", yytext); listIndex++;}
(\<.+\>)|(\[.+\]\(.+\))			{printf("`%s` is a link\n", yytext);}
\\[\!`_*{}()<>[\]#+-.|]{1,5}	{printf("`%s` is an escape sequence\n", yytext);}
%%
 
/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	return 0;
}