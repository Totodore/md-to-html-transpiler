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


\*\*.+\*\*						{printf("%s BOLD\n", yytext);}
\*.+\*							{printf("%s ITALIC\n", yytext);}
\__.+\__						{printf("%s UNDERLINED\n", yytext);}
\~~.+\~~				    	{printf("%s STRIKETHROUGH\n", yytext);}
[-][-][-]						{printf("%s HR\n", yytext);}
^#[ ].+\n 			        	{printf("%s H1 \n", yytext);}
^##[ ].+\n   					{printf("%s H2 \n", yytext);}
^###[ ].+\n 					{printf("%s H3 \n", yytext);}
^####[ ].+\n    				{printf("%s H4 \n", yytext);}
^#####[ ].+\n 					{printf("%s H5 \n", yytext);}
^######[ ].+\n     				{printf("%s H6 \n", yytext);}

\n 								{printf("NEWLINE\n");}
(\*\*\*.+\*\*\*)|(___.+___)		{printf("`%s` BOLD ITALIC\n", yytext);}
^[ ]{,3}"* ".+					{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]{,3}"- ".+					{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]{,3}[1-9][0-9]*". ".+		{printf("`%s` LIST ELEMENT\n", yytext); listIndex++;}
(\<.+\>)|(\[.+\]\(.+\))			{printf("`%s` LINK\n", yytext);}
\\[\!`_*{}()<>[\]#+-.|]{1,5}	{printf("`%s` ESCAPE_SEQ\n", yytext);}
%%
 
/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	return 0;
}
