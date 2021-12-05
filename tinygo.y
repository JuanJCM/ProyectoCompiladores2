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
%token KW_PRINT

%%
input:start_input input external_declaration { printf("Finished Parsing\n"); }
    |external_declaration
    ;

start_input: KW_PACKAGE TK_IDENT import { printf("Start input\n"); }
    ;
import: KW_IMPORT TK_STRING
    | KW_IMPORT OP_OPEN_PAR ident_list OP_CLOSE_PAR
    ;
ident_list: TK_STRING ident_list
    |TK_STRING
    ;

external_declaration: method_definition { printf("External declaration\n"); }
    |declaration_list
    |
    ;
    

method_definition: KW_FUNC TK_IDENT OP_OPEN_PAR parameters_type_list OP_CLOSE_PAR OP_OPEN_BRACKET OP_CLOSE_BRACKET brackets type  block_statement { printf("method definition 1\n"); }
    | KW_FUNC TK_IDENT OP_OPEN_PAR parameters_type_list OP_CLOSE_PAR block_statement { printf("method definition 2\n"); }
    | KW_FUNC TK_IDENT OP_OPEN_PAR OP_CLOSE_PAR brackets type  block_statement { printf("method definition 2\n"); }
    | KW_FUNC TK_IDENT OP_OPEN_PAR OP_CLOSE_PAR block_statement { printf("method definition 4\n"); }
    ;

declaration_list: declaration_list declaration { printf("Declaration list\n"); }
    | declaration { printf("Declaration list -> declaration\n"); }
    |
    ;

declaration: KW_VAR TK_IDENT OP_EQ TK_STRING { printf("Declaration -> string\n"); }
    | KW_VAR TK_IDENT OP_EQ TK_NUMBER { printf("Declaration -> number\n"); }
    | KW_VAR TK_IDENT OP_EQ bool    { printf("Declaration -> bool\n"); }
    | KW_VAR TK_IDENT type
    | KW_VAR TK_IDENT type OP_EQ TK_NUMBER
    | print_statement
    | TK_IDENT OP_COLON_EQ decl_types
    | TK_IDENT OP_COLON_EQ TK_IDENT '.' TK_IDENT OP_OPEN_PAR print_list OP_CLOSE_PAR optional
    | TK_IDENT OP_EQ expression
    | TK_IDENT OP_COLON_EQ brackets type list_data
    | array OP_EQ array
    | array OP_EQ decl_types
    ;

array: TK_IDENT OP_OPEN_BRACKET array_params OP_CLOSE_BRACKET
    ;
    
array_params: TK_IDENT 
    | TK_NUMBER
    | TK_IDENT relational_operands array_params
    ;

relational_operands: OP_PLUS
    | OP_MINUS
    | OP_MULT
    | OP_DIV
    ;
    
optional: optional operators
    | operators
    ;

operators: optional_operands decl_types { printf("operator -> plus\n"); }
    |
    ;
optional_operands: OP_PLUS 
    | OP_MINUS
    | OP_DIV
    | OP_MULT 
    ;

bool: KW_TRUE
    | KW_FALSE 
    ;

type: DTYPE_INT
    | DTYPE_BOOL
    | DTYPE_FLOAT32
    |
    ;
    
brackets: OP_OPEN_BRACKET OP_CLOSE_BRACKET  
    |
    ;

init_declarator_list: init_declarator_list OP_COMMA init_declarator_list
    |init_declarator
    ;

init_declarator: declarator
    |declarator OP_COLON_EQ initializer
    ;

declarator: TK_IDENT brackets type
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

block_statement: OP_OPEN_BRACES statement_list OP_CLOSE_BRACES { printf("Block statement\n"); }
    |
    ;

statement_list: statement_list statement { printf("Statement list\n"); }
    |statement { printf("Statement list -> statement\n"); }
    ;

statement: for_statement
    |if_statement
    |jump_statement
    |expression_statement
    |assignation_statement
    |print_statement { printf("Statement -> print\n"); }
    ;

assignation_statement: declaration
    ;
    
print_statement: TK_IDENT '.' KW_PRINT OP_OPEN_PAR print_list OP_CLOSE_PAR { printf("Print Statement 2\n"); }
    ;

print_list: print_list expression print_extra
    | print_list TK_STRING print_extra
    | print_list bool print_extra
    | print_list TK_IDENT print_extra
    | 
    ;

print_extra:
    | OP_COMMA decl_types
    | OP_COMMA expression
    | method_call
    ;

method_call: OP_OPEN_PAR call_method_list OP_CLOSE_PAR
    ;    

call_method_list: TK_IDENT
    | TK_IDENT OP_COMMA call_method_list
    | 
    ;    

decl_types:TK_IDENT
    |TK_STRING
    | bool
    |TK_NUMBER
    ;

assignment_expression: unary_expression assignment_expression
    |logical_or_expression
    ;

postfix_expression: primary_expression
                |postfix_expression OP_OPEN_BRACKET expression OP_CLOSE_BRACKET
                |postfix_expression OP_OPEN_PAR OP_CLOSE_PAR
                |postfix_expression OP_OPEN_PAR argument_expression_list OP_CLOSE_PAR
                |postfix_expression OP_PLUS_PLUS
                |postfix_expression OP_MINUS_MINUS
    ;

primary_expression: OP_OPEN_PAR expression OP_CLOSE_PAR
    | constant
    ;

constant: TK_IDENT
    | TK_FLOAT32
    | TK_NUMBER 
    | bool
    ;

list_data: OP_OPEN_BRACES lists OP_CLOSE_BRACES OP_SEMICOLON
    |OP_OPEN_BRACES lists OP_CLOSE_BRACES
    |
    ;
    
lists: lists OP_COMMA constant 
    |constant
    |
    ;

argument_expression_list: argument_expression_list OP_SEMICOLON assignment_expression
    | assignment_expression
    ;

if_statement: KW_IF expression_list OP_OPEN_BRACES statement_list OP_CLOSE_BRACES 
    |KW_IF expression_list OP_OPEN_BRACES statement_list OP_CLOSE_BRACES else_statement
    ;

else_statement:KW_ELSE if_statement
    |KW_ELSE OP_OPEN_BRACES statement_list OP_CLOSE_BRACES
    ;

for_statement: KW_FOR expression_list OP_OPEN_BRACES statement_list OP_CLOSE_BRACES  
    | KW_FOR OP_OPEN_BRACES statement_list OP_CLOSE_BRACES
    ;

jump_statement: KW_RETURN
    |KW_CONTINUE
    |KW_BREAK
    ;

expression_list: expression OP_SEMICOLON expression_list
    |expression OP_AND_AND expression_list
    |expression OP_PIPE_PIPE expression_list
    |expression
    ;
    
expression_statement: expression {printf( "Expression statement\n" );}
    ;

logical_or_expression: logical_or_expression
    |logical_and_expression
    ;

logical_and_expression: logical_and_expression
    |equality_expression
    ;

equality_expression: equality_expression OP_COLON_EQ brackets relational_expression list_data
    |equality_expression OP_EXCLAMATION_EQ relational_expression
    |equality_expression OP_COLON_EQ bool
    |relational_expression
    |
    ;

compare_expression: OP_AND_AND compare_expression
    | OP_PIPE_PIPE compare_expression
    |OP_PERCENT compare_expression
    | OP_EQ_EQ compare_expression
    | postfix_expression
    ;

unary_expression: OP_PLUS_PLUS unary_expression 
                | OP_MINUS_MINUS unary_expression 
                | OP_EXCLAMATION unary_expression
                | compare_expression
                ;

multiplicative_expression: multiplicative_expression OP_MULT unary_expression 
      | multiplicative_expression OP_DIV unary_expression 
      | unary_expression 
      ;

additive_expression:  additive_expression OP_PLUS multiplicative_expression { printf("Add expression =>  PLUS Expr\n"); }
                    | additive_expression OP_MINUS multiplicative_expression { printf("Add expresison => SUB Expr\n"); }
                    | multiplicative_expression { printf("Add expression => mult expr\n"); }
                    ;

relational_expression: relational_expression OP_GREATER_THAN additive_expression 
                     | relational_expression OP_LESS_THAN additive_expression 
                     | relational_expression OP_GREATER_THAN_EQ additive_expression 
                     | relational_expression OP_LESS_THAN_EQ additive_expression 
                     | additive_expression { printf("relation expr => add expr\n"); }
                     ;

expression: assignment_expression
;