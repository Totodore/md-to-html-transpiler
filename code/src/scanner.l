/*** Definition Section for global state ***/
%{
	int count = 0;
%}

/*** Rules Section ***/
%%

\*\*.+\*\*					{printf("%s BOLD\n", yytext);}
\*.+\*						{printf("%s ITALIC\n", yytext);}
\__.+\__					{printf("%s UNDERLINED\n", yytext);}
\∼∼.+\∼∼				   {printf("%s STRIKETHROUGH\n", yytext);}
[-][-][-]					{printf("%s HR\n", yytext);}
^#[ ].+\n 			        {printf("%s H1 \n", yytext);}
^##[ ].+\n   				{printf("%s H2 \n", yytext);}
^###[ ].+\n 				{printf("%s H3 \n", yytext);}
^####[ ].+\n    			{printf("%s H4 \n", yytext);}
^#####[ ].+\n 				{printf("%s H5 \n", yytext);}
^######[ ].+\n     			{printf("%s H6 \n", yytext);}
%%
 
/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	printf("\n");
	return 0;
}
