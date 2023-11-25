/*** Definition Section for global state ***/
%{
	int count = 0;
	int listIndex = 0;
%}

COLOR (([a-z]+)|("#"[0-9a-fA-F]{6}))
DIGIT [0-9]
NUMBER DIGIT*
COORDINATE {NUMBER},{NUMBER}

%x XSVG

/*** Rules Section ***/
%%
\!\[.+\]\(.+\)					{ printf("%s IMAGE\n", yytext); }
\>.+							{ printf("%s QUOTE\n", yytext); }
^\`\`\`\n[^`][^`][^`]*\`\`\`	{ printf("%s BLOCK_CODE\n", yytext); }
\`[^\n\`]\`						{ printf("%s INLINE_CODE\n", yytext); }
\*\*\*+|\-\-\-+|\_\_\_+			{ printf("%s HR\n", yytext); }
(\>\.*\n)+                   	{ printf("%s QUOTE\n", yytext); }


\*\*[^*].+[^*]\*\*				{printf("%s BOLD\n", yytext);}
\*[^*].+[^*]\*					{printf("%s ITALIC\n", yytext);}
\__[^_]+\__						{printf("%s UNDERLINED\n", yytext);}
\~~.+\~~				    	{printf("%s STRIKETHROUGH\n", yytext);}
^#[ ].+\n 			        	{printf("%s H1 \n", yytext);}
^##[ ].+\n   					{printf("%s H2 \n", yytext);}
^###[ ].+\n 					{printf("%s H3 \n", yytext);}
^####[ ].+\n    				{printf("%s H4 \n", yytext);}
^#####[ ].+\n 					{printf("%s H5 \n", yytext);}
^######[ ].+\n     				{printf("%s H6 \n", yytext);}

\n 								{printf("NEWLINE\n");}
(\*\*\*.+\*\*\*)|(___.+___)		{printf("`%s` BOLD ITALIC\n", yytext);}
^[ ]*"* ".+						{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]*"- ".+						{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]*[1-9][0-9]*". ".+			{printf("`%s` LIST ELEMENT\n", yytext); listIndex++;}
(\<.+\>)|(\[.+\]\(.+\))			{printf("`%s` LINK\n", yytext);}
\\[\!`_*{}()<>[\]#+-.|]{1,5}	{printf("`%s` ESCAPE_SEQ\n", yytext);}

^\`\`\`xsvg:{COORDINATE},{COORDINATE}\n		{
	printf("%s XSVG\n", yytext);
	BEGIN(XSVG); 
}
<XSVG>^"line "{COORDINATE}" "{COORDINATE}(" "{COLOR})?\n {
	printf("`%s` LINE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"circle "{COORDINATE}" "{NUMBER}(" "{COLOR})?\n {
	printf("`%s` CIRCLE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"ellipse "{COORDINATE}" "{NUMBER}" "{NUMBER}(" "{COLOR}){0,2}\n {
	printf("`%s` ELLIPSE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>{COORDINATE}								{printf("`%s` COORDINATE\n", yytext);}
<XSVG>{NUMBER}									{printf("`%s` NUMBER\n", yytext);}
<XSVG>" "{COLOR}								{printf("`%s` COLOR\n", yytext);}

<XSVG>^\`\`\`									{
	printf("%s XSVG END\n", yytext);
	BEGIN(INITIAL);
}
%%

/*** 
CIRCLES
ELLIPSES
***/
/*** Code Section prints the number of
capital letter present in the given input ***/
int yywrap(){}

int main() {
	yylex();
	return 0;
}
