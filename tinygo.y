%code requires{
    #include "ast.h"
}

%{
//http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
    #include <cstdio>
    #include "asm.h"
    #include <fstream>
    #include <iostream>
    #include <string>

    using namespace std;
    int yylex();
    extern int yylineno;
    void yyerror(const char * s){
        fprintf(stderr, "Line: %d, error: %s\n", yylineno, s);
    }

    #define YYERROR_VERBOSE 1
    #define YYDEBUG 1
    #define EQUAL 1
    #define PLUSEQUAL 2
    #define MINUSEQUAL 3

    Asm assemblyFile;

    void writeFile(string name){
        ofstream file;
        file.open(name);
        file << assemblyFile.data << endl
        << assemblyFile.global <<endl
        << assemblyFile.text << endl;
        file.close();
    }

    int getArrayType(int type){
        switch(type){
            case 1:
                return 5;
            case 2:
                return 6;
            case 3:
                return 7;
            case 4:
                return 8;
        }

        return 0;
    }

%}

%union{
    const char * string_t;
    int int_t;
    float float_t;
    Expr * expr_t;
    Value * value_t;
    bool bool_t;
    ArgumentList * argument_list_t;
    Statement * statement_t;
    StatementList * statement_list_t;
    InitDeclaratorList * init_declarator_list_t;
    InitDeclarator * init_declarator_t;
    Declarator * declarator_t;
    Initializer * initializer_t;
    InitializerList * initializer_list_t;
    Declaration * declaration_t;
    DeclarationList * declaration_list_t;
    Parameter * parameter_t;
    ParameterList * parameter_list_t;
    ValueList * left_value_list_t;
}

%token OP_ADD OP_SUB OP_MUL OP_CARET OP_DIV OP_PERCENT
%token ADD_ADD SUB_SUB ADD_EQUAL SUB_EQUAL MUL_EQUAL CARET_EQUAL DIV_EQUAL PERCENT_EQUAL
%token AND_EQUAL AND_AND OR_EQUAL PIPE_PIPE COLON_EQUAL GREATER_EQUAL LESS_EQUAL EQUAL_EQUAL EXCLAMATION_EQUAL
%token BREAK FUNC ELSE PACKAGE IF CONTINUE FOR IMPORT RETURN VAR
%token<bool_t> TK_TRUE TK_FALSE
%token<string_t> TK_ID TK_STRING
%token<float_t> TK_FLOAT32
%token<int_t> TK_INT
%token EOL
%token INTEGER BOOLEAN STRING_TYPE FLOAT32

%type<expr_t> expression
%type<argument_list_t> expression_list_option expression_list
%type<statement_list_t> input statement_list function_list 
%type<int_t> types type_option
%type<parameter_list_t> params_list params_list_option
%type<statement_t> function
%type<statement_t> block_statement statement
%type<statement_t> if_statement for_statement 
%type<value_t> value
%type<left_value_list_t> value_list

%left AND_AND PIPE_PIPE OP_PERCENT OP_ADD OP_SUB OP_MUL OP_DIV
%nonassoc '<' '>' GREATER_EQUAL LESS_EQUAL EQUAL_EQUAL EXCLAMATION_EQUAL
%left '!'
%right OP_CARET

%%
start: input {
   assemblyFile.global = ".globl main";
    assemblyFile.data = ".data\n";
    assemblyFile.text = ".text\n";
    list<Statement *>::iterator it = $1->begin();
    string code;
    while(it != $1->end()){
        printf("semantic result: %d \n",(*it)->evaluateSemantic());
        code += (*it)->genCode();
        it++;
    }
    assemblyFile.text += code;
    writeFile("result.s");
};

input: eols_option PACKAGE TK_ID eols import_list eols function_list  { $$ = $7; };

import_list: import_list eols IMPORT TK_STRING {}
    | import_list eols IMPORT '(' eols_option import_names eols_option ')' {}
    | IMPORT TK_STRING {}
    | IMPORT '(' eols_option import_names eols_option ')' {}
    ;

import_names: import_names eols TK_STRING 
    | TK_STRING;

eols: eols EOL 
    | EOL ;

eols_option: eols 
    | ;

end_statement: ';' | EOL ;

types: INTEGER { $$ = INT; } 
    | BOOLEAN { $$ = BOOL;} 
    | STRING_TYPE { $$ = STRING; } 
    | FLOAT32 { $$ = FLOAT; };

params_list: params_list ',' TK_ID types 
            {
                $$ = $1;
                Declarator * dec = new Declarator($3, NULL, false, yylineno);
                Parameter * param = new Parameter((Type)$4, dec, false, yylineno);
                $$->push_back(param);
            }
        | params_list ',' TK_ID '[' ']' types 
            {
                $$ = $1;
                Declarator * dec = new Declarator($3, NULL, false, yylineno);
                Parameter * param = new Parameter((Type)$6, dec, true, yylineno);
                $$->push_back(param);
            }
        | TK_ID types 
            {
                $$ = new ParameterList; 
                Declarator * dec = new Declarator($1, NULL, false, yylineno);
                Parameter * param = new Parameter((Type)$2, dec, false, yylineno);
                $$->push_back(param);
            }
        | TK_ID '[' ']' types 
            {
                $$ = new ParameterList; 
                Declarator * dec = new Declarator($1, NULL, false, yylineno);
                Parameter * param = new Parameter((Type)$4, dec, true, yylineno);
                $$->push_back(param);
            }
;

params_list_option: params_list { $$ = $1;} 
    | { $$ = new ParameterList; } ;

type_option: types { $$ = $1; }| '[' ']' types { $$ = getArrayType($3); } | { $$ = VOID; } ;

function_list: function_list function { $$ = $1; $$->push_back($2); }
    | function { $$ = new StatementList; $$->push_back($1); };

function: FUNC TK_ID '(' params_list_option ')' type_option '{' eols_option block_statement eols_option '}' eols_option 
    { 
        $$ = new MethodDefinition((Type)$6, $2, *$4, $9, yylineno );
        delete $4;
    } ;

block_statement: statement_list 
    { 
        DeclarationList * list = new DeclarationList();
        $$ = new BlockStatement(*$1, *list, yylineno);
        delete list; 
    };

statement_list: statement_list statement eols_option { $$ = $1; $$->push_back($2); }
    | statement eols_option { $$ = new StatementList; $$->push_back($1); };

value: TK_ID { $$ = new Value($1, NULL, yylineno); } | TK_ID '[' expression ']' {  $$ = new Value($1, $3, yylineno); };

value_list: value_list ',' value {  $$ = $1;  $$->push_back($3);} 
    | value { $$ = new ValueList; $$->push_back($1); };

statement: VAR value_list types '=' expression_list end_statement { $$ = new CreateVariableStatement((Type)$3, *$2, *$5,yylineno); }
    | VAR value_list types { $$ = new CreateVariableStatement((Type)$3, *$2, *(new ArgumentList), yylineno); }
    | VAR value_list '=' expression_list end_statement { $$ = new CreateVariableStatement((Type)0, *$2, *$4,yylineno); }
    | value_list '=' expression_list end_statement { $$ = new AssignmentStatement(*$1, *$3, yylineno); }
    | value_list COLON_EQUAL expression_list end_statement { $$ = new CreateVariableStatement((Type)0, *$1, *$3,yylineno); }
    | if_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | RETURN expression end_statement { $$ = new ReturnStatement($2, yylineno); }
    | BREAK end_statement { $$ = new BreakStatement(yylineno); }
    | CONTINUE end_statement { $$ = new ContinueStatement(yylineno); }
    | TK_ID ADD_ADD
        {
            Expr * expression = new AddExpr(new IdExpr($1, yylineno), new IntExpr(1 , yylineno), yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID SUB_SUB
        {
            Expr * expression = new SubExpr(new IdExpr($1, yylineno), new IntExpr(1 , yylineno), yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID ADD_EQUAL expression
        {
            Expr * expression = new AddExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID SUB_EQUAL expression
        {
            Expr * expression = new SubExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID MUL_EQUAL expression
        {
            Expr * expression = new MulExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID CARET_EQUAL expression
        {
            Expr * expression = new ExpExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID DIV_EQUAL expression
        {
            Expr * expression = new DivExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID PERCENT_EQUAL expression
        {
            Expr * expression = new ModExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID AND_EQUAL expression
        {
            Expr * expression = new LogicalAndExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID OR_EQUAL expression
        {
            Expr * expression = new LogicalOrExpr(new IdExpr($1, yylineno), $3, yylineno);
            Value * value = new Value($1, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back(expression);

            $$ = new AssignmentStatement(*valueList, *arglist, yylineno);
        }
    | TK_ID '(' expression_list_option ')' { $$ = new ExprStatement( new MethodInvocationExpr(new IdExpr($1,yylineno), *$3, yylineno), yylineno); }
    | TK_ID '.' TK_ID '(' expression_list_option ')' { $$ = new ExprStatement( new MethodInvocationExpr(new IdExpr(string($1)+"."+string($3), yylineno), *$5, yylineno), yylineno); };

if_statement: IF expression '{' eols_option block_statement eols_option '}' { $$ = new IfStatement($2, $5, yylineno); }
    | IF expression '{' eols_option block_statement eols_option '}' ELSE '{' eols_option block_statement eols_option '}' { $$ = new ElseStatement($2, $5, $11, yylineno); } ;

for_statement: FOR TK_ID COLON_EQUAL expression ';' expression ';' TK_ID ADD_ADD '{' eols_option block_statement eols_option '}'
        {
            Value * value = new Value($2, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back($4);

            CreateVariableStatement *init = new CreateVariableStatement((Type)0, *valueList, *arglist, yylineno);

            Expr * expression = new AddExpr(new IdExpr($8, yylineno), new IntExpr(1 , yylineno), yylineno);
            Value * valueInc = new Value($8, NULL, yylineno);
            ValueList * valueListInc = new ValueList;
            valueListInc->push_back(valueInc);

            ArgumentList* arglistInc = new ArgumentList; 
            arglistInc->push_back(expression);

            AssignmentStatement* increment = new AssignmentStatement(*valueListInc, *arglistInc, yylineno);

            $$ = new ForStatement(init, $6, increment, $12, yylineno);
        }
    | FOR TK_ID COLON_EQUAL expression ';' expression ';' TK_ID SUB_SUB '{' eols_option block_statement eols_option '}'
        {
            Value * value = new Value($2, NULL, yylineno);
            ValueList * valueList = new ValueList;
            valueList->push_back(value);

            ArgumentList* arglist = new ArgumentList; 
            arglist->push_back($4);

            CreateVariableStatement *init = new CreateVariableStatement((Type)0, *valueList, *arglist, yylineno);

            Expr * expression = new SubExpr(new IdExpr($8, yylineno), new IntExpr(1 , yylineno), yylineno);
            Value * valueInc = new Value($8, NULL, yylineno);
            ValueList * valueListInc = new ValueList;
            valueListInc->push_back(valueInc);

            ArgumentList* arglistInc = new ArgumentList; 
            arglistInc->push_back(expression);

            AssignmentStatement* increment = new AssignmentStatement(*valueListInc, *arglistInc, yylineno);

            $$ = new ForStatement(init, $6, increment, $12, yylineno);
        }
    | FOR expression '{' eols_option block_statement eols_option '}' { $$ = new ForsStatement($2, $5, yylineno); }
    | FOR '{' eols_option block_statement eols_option '}' 
        { 
            BoolExpr * expression = new BoolExpr(true, yylineno);
            $$ = new ForsStatement(expression, $4, yylineno);
        }
    ;

expression_list: expression_list ',' expression { $$ = $1;  $$->push_back($3);}
        | expression_list ',' '[' ']' types '{' expression_list'}' { $$ = $1;  $$->push_back(new ArrayInitExpr(*$7,(Type)$5,yylineno)); }
        | '[' ']' types '{' expression_list'}' { $$ = new ArgumentList; $$->push_back(new ArrayInitExpr(*$5,(Type)$3,yylineno)); }
        | expression  {$$ = new ArgumentList; $$->push_back($1);}
        
;

expression_list_option: expression_list {$$ = $1;} | {$$ = new ArgumentList;} ;

expression: expression '<' expression { $$ = new LtExpr($1, $3, yylineno); }
    | expression '>' expression { $$ = new GtExpr($1, $3, yylineno); }
    | expression LESS_EQUAL expression { $$ = new LteExpr($1, $3, yylineno); }
    | expression GREATER_EQUAL expression { $$ = new GteExpr($1, $3, yylineno); }
    | expression EXCLAMATION_EQUAL expression { $$ = new NeqExpr($1, $3, yylineno); }
    | expression EQUAL_EQUAL expression { $$ = new EqExpr($1, $3, yylineno); }
    | expression AND_AND expression { $$ = new LogicalAndExpr($1, $3, yylineno); }
    | expression PIPE_PIPE expression { $$ = new LogicalOrExpr($1, $3, yylineno); }
    | expression OP_ADD expression { $$ = new AddExpr($1, $3, yylineno); }
    | expression OP_SUB expression { $$ = new SubExpr($1, $3, yylineno); }
    | expression OP_DIV expression { $$ = new DivExpr($1, $3, yylineno); }
    | expression OP_MUL expression { $$ = new MulExpr($1, $3, yylineno); }
    | expression OP_PERCENT expression { $$ = new ModExpr($1, $3, yylineno); }
    | expression OP_CARET expression { $$ = new ExpExpr($1, $3, yylineno); }
    | '!' expression { $$ = $$ = new UnaryExpr(NOT, $2, yylineno);}
    | TK_ID { $$ = new IdExpr($1, yylineno); }
    | TK_FLOAT32 { $$ = new FloatExpr($1 , yylineno); }
    | TK_INT { $$ = new IntExpr($1 , yylineno); }
    | TK_STRING { $$ = new StringExpr($1, yylineno); }
    | TK_FALSE { $$ = new BoolExpr($1, yylineno); }
    | TK_TRUE { $$ = new BoolExpr($1, yylineno); }
    | '(' expression ')' { $$ = $2; }
    | TK_ID '[' expression ']' { $$ = new ArrayExpr(new IdExpr($1,yylineno), $3, yylineno); }
    | TK_ID '(' expression_list_option ')' { $$ = new MethodInvocationExpr(new IdExpr($1,yylineno), *$3, yylineno); }
    | TK_ID '.' TK_ID '(' expression_list_option ')' { $$ = new MethodInvocationExpr(new IdExpr($1,yylineno), new IdExpr($3,yylineno), *$5, yylineno); }
    ;
%%
