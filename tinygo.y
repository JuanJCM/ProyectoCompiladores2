%{
    #include <iostream>
    #include <cstdio>
    using namespace std;
    int yylex();
    extern int yylineno;
    void yyerror(const char *msg) {
        printf("Error at line: %d, error: %s\n", yylineno, msg);
    }
    #define YYERROR_VERBOSE 1
%}

%token OP_PLUS OP_PLUS_EQ OP_PLUS_PLUS
%token OP_MINUS OP_MINUS_EQ OP_MINUS_MINUS
%token OP_MULT OP_MULT_EQ
%token OP_DIV OP_DIV_EQ
%token OP_EQ OP_EQ_EQ
%token OP_PIPE_PIPE OP_PIPE_EQ
%token OP_GREATER_THAN OP_GREATER_THAN_EQ
%token OP_LESS_THAN OP_LESS_THAN_EQ
%token OP_CARET OP_CARET_EQ
%token OP_AND_AND OP_AND_EQUAL
%token OP_EXCLAMATION OP_EXCLAMATION_EQ
%token OP_OPEN_PAR OP_CLOSE_PAR OP_OPEN_BRACKET OP_CLOSE_BRACKET OP_OPEN_BRACES OP_CLOSE_BRACES
%token OP_COLON OP_COLON_EQ OP_SEMICOLON OP_COMMA OP_PERCENT OP_PERCENT_EQ
%token DTYPE_INT DTYPE_BOOL DTYPE_FLOAT32
%token KW_BREAK KW_FUNC KW_ELSE KW_PACKAGE KW_IF KW_CONTINUE
%token KW_FOR KW_IMPORT KW_RETURN KW_VAR KW_TRUE KW_FALSE
%token TK_IDENT TK_NUMBER TK_FLOAT32 TK_STRING

%%
input: input external_declaration
    |external_declaration
    ;

external_declaration: method_definition
    |declaration
    ;

method_definition: KW_FUNC TK_IDENT OP_OPEN_PAR parameters_type_list OP_CLOSE_PAR type  block_statement 
    | KW_FUNC TK_IDENT OP_OPEN_PAR parameters_type_list OP_CLOSE_PAR  block_statement 
    | KW_FUNC TK_IDENT OP_OPEN_PAR OP_CLOSE_PAR type  block_statement 
    | KW_FUNC TK_IDENT OP_OPEN_PAR OP_CLOSE_PAR block_statement 
    ;
declaration_list: declaration_list declaration
    | declaration
    ;

declaration: type init_declarator_list
    ;

type: DTYPE_INT
    | DTYPE_BOOL
    | DTYPE_FLOAT32
    ;

init_declarator_list: init_declarator_list OP_COMMA init_declarator_list
    |init_declarator
    ;

init_declarator: declarator
    |declarator OP_COLON_EQ initializer
    ;
declarator: TK_IDENT
    |TK_IDENT OP_OPEN_BRACKET assignment_expression OP_CLOSE_BRACKET
    |TK_IDENT OP_OPEN_BRACKET OP_CLOSE_BRACKET
    ;
parameters_type_list: parameters_type_list OP_COMMA parameter_declaration
    | parameter_declaration
    ;
parameter_declaration: type declarator
    | OP_OPEN_BRACKET OP_CLOSE_BRACKET type
    ;

initializer: assignment_expression
    ;

block_statement: OP_OPEN_BRACES statement_list OP_CLOSE_BRACES
    |
    ;
statement_list: statement_list statement
    |statement
    ;
statement: for_statement
    |if_statement
    |jump_statement
    |expression_statement
    |block_statement
    ;
assignment_expression:unary_expression assignment_expression
    |logical_or_expression
    ;


if_statement: KW_IF expression statement
    ;
for_statement: KW_FOR expression_statement
    ;
jump_statement: KW_RETURN
    |KW_CONTINUE
    |KW_BREAK
    ;
expression_statement:expression
    ;
logical_or_expression: logical_or_expression
    |logical_and_expression
    ;
logical_and_expression: logical_and_expression
    |equality_expression
    ;

equality_expression: equality_expression OP_COLON_EQ relational_expression
    |equality_expression OP_EXCLAMATION_EQ relational_expression
    |relational_expression
    ;

unary_expression: OP_PLUS_PLUS unary_expression 
                | OP_MINUS_MINUS unary_expression 
                | OP_EXCLAMATION unary_expression  
                ;

multiplicative_expression: multiplicative_expression OP_MULT unary_expression 
      | multiplicative_expression OP_DIV unary_expression 
      | unary_expression 
      ;

additive_expression:  additive_expression OP_PLUS multiplicative_expression
                    | additive_expression OP_MINUS multiplicative_expression 
                    | multiplicative_expression 
                    ;

relational_expression: relational_expression OP_GREATER_THAN additive_expression 
                     | relational_expression OP_LESS_THAN additive_expression 
                     | relational_expression OP_GREATER_THAN_EQ additive_expression 
                     | relational_expression OP_LESS_THAN_EQ additive_expression 
                     | additive_expression 
                     ;
expression: assignment_expression
;