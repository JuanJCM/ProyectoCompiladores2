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
"+" { return OP_ADD; }
"-" { return OP_SUB; }
"*" { return OP_MUL; }
"^" { return OP_CARET; }
"/" { return OP_DIV; }
"%" { return OP_PERCENT; }
"++" { return ADD_ADD; }
"--" { return SUB_SUB; }
"+=" { return ADD_EQUAL; }
"-=" { return SUB_EQUAL; }
"*=" { return MUL_EQUAL; }
"^=" { return CARET_EQUAL; }
"/=" { return DIV_EQUAL; }
"%=" { return PERCENT_EQUAL; }
"&=" { return AND_EQUAL; }
"&&" { return AND_AND; }
"|=" { return OR_EQUAL; }
"||" { return PIPE_PIPE; }
"=" { return '='; }
":=" { return COLON_EQUAL; }
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
">=" { return GREATER_EQUAL; }
"<=" { return LESS_EQUAL; }
"==" { return EQUAL_EQUAL; }
"!=" { return EXCLAMATION_EQUAL; }
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
. { printf("Invalid token %d\n", (int)(yytext[0])); }
%%
