%code requires{

}

%{
    
%}

%union{

}
%token fUNC
%token ID STRING_TYPE
%token INT_TYPE
%token FLOAT32_TYPE
%token BOOL_TYPE
%token OP_COLON_EQ OPEN_PAR CLOSE_PAR OPEN_BRACES CLOSE_BRACES OPEN_BRACKET CLOSE_BRACKET
%token OP_COMMA OP_EXCLAMATION_EQ OP_PLUS_PLUS OP_MINUS_MINUS OP_EXCLAMATION
%token OP_GREATER_THAN OP_LESS_THAN
%token KW_FOR KW_IF
%%
input: input external_declaration
    |external_declaration
    ;

external_declaration: method_definition
    |declaration
    ;

method_definition: fUNC ID OPEN_PAR parameters_type_list CLOSE_PAR type  block_statement 
    | fUNC ID OPEN_PAR parameters_type_list OPEN_PAR  block_statement 
    |fUNC ID OPEN_PAR CLOSE_PAR type  block_statement 
    |fUNC ID OPEN_PAR CLOSE_PAR block_statement 
    ;
declaration_list: declaration_list declaration
    | declaration
    ;

declaration: type init_declarator_list
    ;

type: STRING_TYPE
    | INT_TYPE
    | FLOAT32_TYPE
    | BOOL_TYPE
    ;

init_declarator_list: init_declarator_list OP_COMMA init_declarator_list
    |init_declarator
    ;

init_declarator: declarator
    |declarator OP_COLON_EQ initializer
    ;
declarator: ID
    |ID OPEN_BRACKET assignment_expression CLOSE_BRACKET
    |ID OPEN_BRACKET CLOSE_BRACKET
    ;
parameters_type_list: parameters_type_list OP_COMMA parameter_declaration
    | parameter_declaration
    ;
parameter_declaration: type declarator
    | OPEN_BRACKET CLOSE_BRACKET type
    ;

initializer: assignment_expression
    ;

block_statement: OPEN_BRACES statement_list CLOSE_BRACES
    |
    ;
statement_list: statement_list statement
    |statement
    ;
statement: for_statement
    | if_statement
    | jump_statement
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
jump_statement:
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

multiplicative_expression: multiplicative_expression '*' unary_expression 
      | multiplicative_expression '/' unary_expression 
      | unary_expression 
      ;

additive_expression:  additive_expression '+' multiplicative_expression
                    | additive_expression '-' multiplicative_expression 
                    | multiplicative_expression 
                    ;

relational_expression: relational_expression OP_GREATER_THAN additive_expression 
                     | relational_expression OP_LESS_THAN additive_expression 
                     | relational_expression GREATER_OR_EQUAL additive_expression 
                     | relational_expression LESS_OR_EQUAL additive_expression 
                     | additive_expression 
                     ;
