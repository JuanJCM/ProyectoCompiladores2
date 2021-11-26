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
%token COMMA NOT_EQUAL
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

init_declarator_list: init_declarator_list ',' init_declarator_list
    |init_declarator
    ;

init_declarator: declarator
    |declarator OP_COLON_EQ initializer
    ;
declarator: ID
    |ID'['assignment_expression']'
    |ID '['']'
    ;
parameters_type_list: parameters_type_list',' parameter_declaration
    | parameter_declaration
    ;
parameter_declaration: type declarator
    | '['']' type
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


if_statement:
    ;
for_statement:
    ;
jump_statement:
    ;
expression_statement:
    ;
logical_or_expression: logical_or_expression
    |logical_and_expression
    ;
logical_and_expression: logical_and_expression
    |equality_expression
    ;

equality_expression: equality_expression OP_COLON_EQ relational_expression
    |equality_expression NOT_EQUAL relational_expression
    |relational_expression
    ;

unary_expression: PLUS_PLUS unary_expression 
                | MINUS_MINUS unary_expression 
                | NOT unary_expression  
                ;

multiplicative_expression: multiplicative_expression '*' unary_expression 
      | multiplicative_expression '/' unary_expression 
      | unary_expression 
      ;

additive_expression:  additive_expression '+' multiplicative_expression
                    | additive_expression '-' multiplicative_expression 
                    | multiplicative_expression 
                    ;

relational_expression: relational_expression '>' additive_expression 
                     | relational_expression '<' additive_expression 
                     | relational_expression GREATER_OR_EQUAL additive_expression 
                     | relational_expression LESS_OR_EQUAL additive_expression 
                     | additive_expression 
                     ;
