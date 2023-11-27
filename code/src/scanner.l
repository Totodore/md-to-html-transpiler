/*** Definition Section for global state ***/
%{
	int count = 0;
	int coordinate = -1;
%}

ATTRIBUTE (([a-z]+)|("#"[0-9a-fA-F]{6}))
DIGIT [0-9]
NUMBER {DIGIT}*
COORDINATE {NUMBER},{NUMBER}
STRING (\"[^\"]*\")

%x XSVG 
%x COORDINATE

/*** Rules Section ***/
%%
\!\[.+\]\(.+\)					{ printf("%s IMAGE\n", yytext); }
\>.+							{ printf("%s QUOTE\n", yytext); }
^\`\`\`\n[^`][^`][^`]*\`\`\`	{ printf("%s BLOCK_CODE\n", yytext); }
\`[^\n\`]\`						{ printf("%s INLINE_CODE\n", yytext); }
\*\*\*+|\-\-\-+|\_\_\_+			{ printf("%s HR\n", yytext); }
(\>\.*\n)+                   	{ printf("%s QUOTE\n", yytext); }


\*\*([^*]+)\*\* 				{printf("%s BOLD\n", yytext);}
\*([^*]+)\*					    {printf("%s ITALIC\n", yytext);}
\__[^_]+\__						{printf("%s UNDERLINED\n", yytext);}
\~~.+\~~				    	{printf("%s STRIKETHROUGH\n", yytext);}
^#[ ].+\n 			        	{printf("%s H1 TEXT\n", yytext);}
^##[ ].+\n   					{printf("%s H2 TEXT\n", yytext);}
^###[ ].+\n 					{printf("%s H3 TEXT\n", yytext);}
^####[ ].+\n    				{printf("%s H4 TEXT\n", yytext);}
^#####[ ].+\n 					{printf("%s H5 TEXT\n", yytext);}
^######[ ].+\n     				{printf("%s H6 TEXT\n", yytext);}
^\s*\n 							{printf("BLANK_LINE\n");}
\n 								{printf("NEWLINE\n");}
(\*\*\*.+\*\*\*)|(___.+___)		{printf("`%s` BOLD ITALIC\n", yytext);}
^[ ]*"* ".+						{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]*"- ".+						{printf("`%s` LIST ELEMENT\n", yytext);}
^[ ]*[1-9][0-9]*". ".+			{printf("`%s` LIST ELEMENT\n", yytext);}
(\<.+\>)|(\[.+\]\(.+\))			{printf("`%s` LINK\n", yytext);}
\\[\!`_*{}()<>[\]#+-.|]{1,5}	{printf("`%s` ESCAPE_SEQ\n", yytext);}

^\`\`\`xsvg:{COORDINATE},{COORDINATE}\n		{
	printf("%s XSVG\n", yytext);
	BEGIN(XSVG); 
}
.|\n 							{printf("`%s` TEXT\n", yytext);}
<XSVG>^"line "{COORDINATE}" "{COORDINATE}(" "{ATTRIBUTE})?[ ]*\n {
	printf("`%s` LINE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"polyline"(" "{COORDINATE})+(" "{ATTRIBUTE})?[ ]*\n {
	printf("`%s` POLYLINE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"polygon"(" "{COORDINATE})+(" "{ATTRIBUTE}){0,2}[ ]*\n {
	printf("`%s` POLYGON\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"circle "{COORDINATE}" "{NUMBER}(" "{ATTRIBUTE}){0,2}[ ]*\n {
	printf("`%s` CIRCLE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"ellipse "{COORDINATE}(" "{NUMBER}){2}(" "{ATTRIBUTE}){0,2}[ ]*\n {
	printf("`%s` ELLIPSE\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"rect "{COORDINATE}(" "{NUMBER}){2}(" "{ATTRIBUTE}){0,2}[ ]*\n {
	printf("`%s` RECT\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>^"text "{COORDINATE}" "{STRING}(" "{ATTRIBUTE}){0,2}[ ]*\n {
	printf("`%s` TEXT\n", yytext);
	REJECT; // Rejects the current match so we can match coordinates and color
}
<XSVG>{COORDINATE}								{BEGIN(COORDINATE); REJECT;}
<XSVG>{NUMBER}									{printf("`%s` NUMBER\n", yytext);}
<XSVG>{STRING}									{printf("`%s` STRING\n", yytext);}
<XSVG>{ATTRIBUTE}							    {printf("`%s` ATTRIBUTE\n", yytext);}
<XSVG>\n										{printf("NEWLINE\n");}

<XSVG>^\`\`\`									{
	printf("%s XSVG END\n", yytext);
	BEGIN(INITIAL);
}

<COORDINATE>{NUMBER}							{
	printf("`%s` NUMBER\n", yytext);
	if(coordinate){
		BEGIN(XSVG);
		coordinate = -1;
	} else 
		coordinate = 1;
}
<COORDINATE>","								{printf("`%s` COMMA\n", yytext);}
 
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
	printf("\n");
	return 0;
}