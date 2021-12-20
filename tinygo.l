%option noyywrap
%option yylineno
%option caseless
%x comment

%{
    #include <stdio.h>
    #include "tokens.h"
%}

DIGIT [0-9]
LETTER [a-zA-Z]

%%
"//".* {  }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] { }
\"([^\\\"]|\\.)*\"  { yylval.string_t = strdup(yytext); return TK_STRING;}
{DIGIT}+  { yylval.int_t = atoi(yytext); return TK_INT; }
{DIGIT}+"."{DIGIT}+  { yylval.float_t = atof(yytext); return TK_FLOAT32; }
"+" { return OP_ADD; } /*ADD*/
"-" { return OP_SUB; } /*SUB*/
"*" { return OP_MUL; } /*MUL*/
"^" { return OP_CARET; } /*EXP*/
"/" { return OP_DIV; } /*DIV*/
"%" { return OP_PERCENT; } /*PERC*/
"++" { return ADD_ADD; } /*ADDADD*/
"--" { return SUB_SUB; } /*SUBSUB*/
"+=" { return ADD_EQUAL; } /*IADD*/
"-=" { return SUB_EQUAL; } /*ISUB*/
"*=" { return MUL_EQUAL; } /*IMUL*/
"^=" { return CARET_EQUAL; } /*IEXP*/
"/=" { return DIV_EQUAL; } /*IDIV*/
"%=" { return PERCENT_EQUAL; } /*IPERC*/
"&=" { return AND_EQUAL; } /*IAND*/
"&&" { return AND_AND; } /*AND*/
"|=" { return OR_EQUAL; } /*IOR*/
"||" { return PIPE_PIPE; } /*OR*/
"=" { return '='; }
":=" { return COLON_EQUAL; } /*SHORT*/
"(" { return '('; }
")" { return ')'; }
"[" { return '['; }
"]" { return ']'; }
"{" { return '{'; }
"}" { return '}'; }
"," { return ','; }
";" { return ';'; }
"!" { return '!'; }
":" { return ':'; }
">" { return '>'; }
"<" { return '<'; }
"." { return '.'; }
">=" { return GREATER_EQUAL; } /*GET*/
"<=" { return LESS_EQUAL; } /*LET*/
"==" { return EQUAL_EQUAL; } /*EQT*/
"!=" { return EXCLAMATION_EQUAL; } /*DIST*/
"break" { return BREAK; }
"func" { return FUNC; }
"else" { return ELSE; }
"package" { return PACKAGE; }
"if" { return IF; }
"continue" { return CONTINUE; }
"for" { return FOR; }
"import" { return IMPORT; }
"return" { return RETURN; }
"var" { return VAR; }
"true" { yylval.bool_t = true; return TK_TRUE; }
"false" {  yylval.bool_t = false; return TK_FALSE; }
"int" { return INTEGER; }
"bool" { return BOOLEAN; }
"string" { return STRING_TYPE; }
"float32" { return FLOAT32; }
[a-zA-Z]({DIGIT}|[a-zA-Z_0-9]|_)* { yylval.string_t = strdup(yytext); return TK_ID; }
[\n] { return EOL; }
[ \t\r] { }
. { printf("invalido %d\n", (int)(yytext[0])); }
%%