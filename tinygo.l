%option noyywrap
%option yylineno
%x comment

%{
    #include <stdio.h>
    #include "tokens.h"
    extern int yylval;
%}

DIGIT   [0-9]
STRING \"[^"]*\"|\'[^']*\'
LETTER  [a-zA-Z]
ALPHA_NUMERIC   [a-zA-Z_0-9]

%%

[\t \r \n] {/*Ignore Tabs, EndOfLine Windows, EndOfLine Linux/Unix*/}
"//"[^\n]* {/*Ignore Line Comments*/}
"/*"    {BEGIN(comment); /*Ignore Block Comments*/}
"."     { return '.'; }
"+"     { return OP_PLUS; }
"+="    { return OP_PLUS_EQ; }
"++"    { return OP_PLUS_PLUS; }
"-"     { return OP_MINUS; }
"-="    { return OP_MINUS_EQ; }
"--"    { return OP_MINUS_MINUS; }
"*"     { return OP_MULT; }
"*="    { return OP_MULT_EQ; }
"/"     { return OP_DIV; }
"/="    { return OP_DIV_EQ; }
"="     { return OP_EQ; }
"=="    { return OP_EQ_EQ; }
"||"    { return OP_PIPE_PIPE; }
"|="    { return OP_PIPE_EQ; }
">"     { return OP_GREATER_THAN; }
">="    { return OP_GREATER_THAN_EQ; }
"<"     { return OP_LESS_THAN; }
"<="    { return OP_LESS_THAN_EQ; }
"^"     { return OP_CARET; }
"^="    { return OP_CARET_EQ; }
"&&"    { return OP_AND_AND; }
"&="    { return OP_AND_EQUAL; }
"!"     { return OP_EXCLAMATION; }
"!="    { return OP_EXCLAMATION_EQ; }
"("     { return OP_OPEN_PAR; }
")"     { return OP_CLOSE_PAR; }
"["     { return OP_OPEN_BRACKET; }
"]"     { return OP_CLOSE_BRACKET; }
"{"     { return OP_OPEN_BRACES; }
"}"     { return OP_CLOSE_BRACES; }
":"     { return OP_COLON; }
":="    { return OP_COLON_EQ; }
";"     { return OP_SEMICOLON; }
","     { return OP_COMMA; }
"%"     { return OP_PERCENT; }
"%="    { return OP_PERCENT_EQ; }
"int"   { return DTYPE_INT; }
"bool"  { return DTYPE_BOOL; }
"float32"   { return DTYPE_FLOAT32; }
"break"     { return KW_BREAK; }
"func"      { return KW_FUNC; }
"else"      { return KW_ELSE; }
"package"   { return KW_PACKAGE; }
"if"        { return KW_IF; }
"continue"  { return KW_CONTINUE; }
"for"       { return KW_FOR; }
"import"    { return KW_IMPORT; }
"return"    { return KW_RETURN; }
"var"       { return KW_VAR; }
"true"      { return KW_TRUE; }
"false"     { return KW_FALSE; }
"Println"   {return KW_PRINT;}
{STRING}    { return TK_STRING; }
{LETTER}({DIGIT}|{ALPHA_NUMERIC}|_)* {return TK_IDENT; }
{DIGIT}+ { yylval = atoi(yytext); return TK_NUMBER; }
{DIGIT}+"."{DIGIT}+ { yylval = atof(yytext); return TK_FLOAT32; }

. {printf("Invalid character '%c' at line %d\n", yytext[0], yylineno);}

<comment>"*/" {BEGIN(INITIAL);}
<comment>.|\n {}
<comment><<EOF>> {printf("Incomplete block comment at line %d\n", yylineno); return 0;}

%%
