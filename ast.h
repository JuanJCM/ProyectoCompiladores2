#include <string>
#include <list>
#include <map>
#include <iostream>
#include "code.h"

using namespace std;

class Expr;
class InitDeclarator;
class Declaration;
class Parameter;
class Statement;
class Value;
typedef list<Expr *> InitializerList;
typedef list<InitDeclarator *> InitDeclaratorList;
typedef list<Declaration *> DeclarationList;
typedef list<Parameter *> ParameterList;
typedef list<Statement *> StatementList;
typedef list<Expr *> ArgumentList;
typedef list<Value *> ValueList;

enum StatementKind{
    FORS_STATEMENT,
    FOR_STATEMENT,
    IF_STATEMENT,
    EXPRESSION_STATEMENT,
    RETURN_STATEMENT,
    CONTINUE_STATEMENT,
    BREAK_STATEMENT,
    BLOCK_STATEMENT,
    PRINT_STATEMENT,
    FUNCTION_DEFINITION_STATEMENT,
    GLOBAL_DECLARATION_STATEMENT,
    ELSE_STATEMENT,
    CREATE_VARS_STATEMENT,
    ASSIGN_STATEMENT,
};

enum UnaryType{
    INCREMENT,
    DECREMENT,
    NOT
};

class Statement{
    public:
        int line;
        virtual int evaluateSemantic() = 0;
        virtual StatementKind getKind() = 0;
        virtual string genCode() = 0;
};

class Expr{
    public:
        int line;
        virtual Type getType() = 0;
        virtual void genCode(Code &code) = 0;
};

class Initializer{
    public:
        Initializer(InitializerList expressions, int line){
            this->expressions = expressions;
            this->line = line;
        }
        InitializerList expressions;
        int line;
};

class Declarator{
    public:
        Declarator(string id, Expr* arrayDeclaration, bool isArray, int line){
            this->id = id;
            this->isArray = isArray;
            this->line = line;
            this->arrayDeclaration = arrayDeclaration;
        }
        string id;
        bool isArray;
        int line;
        Expr * arrayDeclaration;
};

class InitDeclarator{
    public:
        InitDeclarator(Declarator * declarator, Initializer * initializer, int line){
            this->declarator = declarator;
            this->initializer = initializer;
            this->line = line;
        }
        Declarator * declarator;
        Initializer * initializer;
        int line;
};

class Declaration{
    public:
        Declaration(Type type, InitDeclaratorList declarations, int line){
            this->type = type;
            this->declarations = declarations;
            this->line = line;
        }
        Type type;
        InitDeclaratorList declarations;
        int line;
        int evaluateSemantic();
        string genCode();
};

class Parameter{
    public:
        Parameter(Type type, Declarator * declarator, bool isArray, int line){
            this->type = type;
            this->declarator = declarator;
            this->line = line;
        }
        Type type;
        Declarator* declarator;
        bool isArray;
        int line;
        int evaluateSemantic();
};

class Value{
    public:
        Value(string id, Expr * expr, int line){
            this->id = id;
            this->expr = expr;
            this->line = line;
        }
        string id;
        Expr * expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class CreateVariableStatement : public Statement{
    public:
        CreateVariableStatement(Type type, ValueList valueList, ArgumentList args, int line){
            this->valueList = valueList;
            this->args = args;
            this->line = line;
            this->type = type;

        }
        ValueList valueList;
        ArgumentList args;
        Type type;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){ return CREATE_VARS_STATEMENT; }
};

class AssignmentStatement : public Statement{
    public:
        AssignmentStatement(ValueList valueList, ArgumentList args, int line){
            this->valueList = valueList;
            this->args = args;
            this->line = line;
        }
        ValueList valueList;
        ArgumentList args;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return ASSIGN_STATEMENT;
        }
};

class ContinueStatement : public Statement{
    public:
        ContinueStatement(int line){
            this->line = line;
        }
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return CONTINUE_STATEMENT;
        }
};

class BreakStatement : public Statement{
    public:
        BreakStatement(int line){
            this->line = line;
        }
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return BREAK_STATEMENT;
        }
};

class BlockStatement : public Statement{
    public:
        BlockStatement(StatementList statements, DeclarationList declarations, int line){
            this->statements = statements;
            this->declarations = declarations;
            this->line = line;
        }
        StatementList statements;
        DeclarationList declarations;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return BLOCK_STATEMENT;
        }
};

class GlobalDeclaration : public Statement {
    public:
        GlobalDeclaration(Declaration * declaration){
            this->declaration = declaration;
        }
        Declaration * declaration;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return GLOBAL_DECLARATION_STATEMENT;
        }
};

class MethodDefinition : public Statement{
    public:
        MethodDefinition(Type type, string id, ParameterList params, Statement * statement, int line){
            this->type = type;
            this->id = id;
            this->params = params;
            this->statement = statement;
            this->line = line;
        }

        Type type;
        string id;
        ParameterList params;
        Statement * statement;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){ return FUNCTION_DEFINITION_STATEMENT; }
};

class IntExpr : public Expr{
    public:
        IntExpr(int value, int line){
            this->value = value;
            this->line = line;
        }
        int value;
        Type getType();
        void genCode(Code &code);
};

class BoolExpr : public Expr{
    public:
        BoolExpr(bool value, int line){
            this->value = value;
            this->line = line;
        }
        bool value;
        Type getType();
        void genCode(Code &code);
};

class FloatExpr : public Expr{
    public:
        FloatExpr(float value, int line){
            this->value = value;
            this->line = line;
        }
        float value;
        Type getType();
        void genCode(Code &code);
};

class BinaryExpr : public Expr{
    public:
        BinaryExpr(Expr * expr1, Expr *expr2, int line){
            this->expr1 = expr1;
            this->expr2 = expr2;
            this->line = line;
        }
        Expr * expr1;
        Expr *expr2;
        int line;
};

#define IMPLEMENT_BINARY_EXPR(name) \
class name##Expr : public BinaryExpr{\
    public: \
        name##Expr(Expr * expr1, Expr *expr2, int line) : BinaryExpr(expr1, expr2, line){}\
        Type getType(); \
        void genCode(Code &code);\
};

class UnaryExpr : public Expr{
    public:
        UnaryExpr(int type, Expr* expr, int line){
            this->type = type;
            this->expr = expr;
            this->line = line;
        }
        int type;
        Expr* expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class PostIncrementExpr: public Expr{
    public:
        PostIncrementExpr(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class PostDecrementExpr: public Expr{
    public:
        PostDecrementExpr(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class IdExpr : public Expr{
    public:
        IdExpr(string id, int line){
            this->id = id;
            this->line = line;
        }
        string id;
        int line;
        Type getType();
        void genCode(Code &code);
};

class ArrayExpr : public Expr{
    public:
        ArrayExpr(IdExpr * id, Expr * expr, int line){
            this->id = id;
            this->expr = expr;
            this->line = line;
        }
        IdExpr * id;
        Expr * expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class MethodInvocationExpr : public Expr{
    public:
        MethodInvocationExpr(IdExpr * id, ArgumentList args, int line){
            this->id = id;
            this->args = args;
            this->line = line;
        }
        MethodInvocationExpr(IdExpr * id, IdExpr * id2, ArgumentList args, int line){
            this->id = id;
            this->id2 = id2;
            this->args = args;
            this->line = line;
        }
        IdExpr * id;
        IdExpr * id2;
        ArgumentList args;
        int line;
        Type getType();
        void genCode(Code &code);
};

class ArrayInitExpr : public Expr{
    public:
        ArrayInitExpr(ArgumentList args, Type type, int line){
            this->args = args;
            this->line = line;
            this->type = type;
        }
        Type type;
        ArgumentList args;
        int line;
        Type getType();
        void genCode(Code &code);
};

class StringExpr : public Expr{
    public:
        StringExpr(string value, int line){
            this->value = value;
            this->line = line;
        }
        string value;
        int line;
        Type getType();
        void genCode(Code &code);
};

class ForsStatement: public Statement{
    public:
        ForsStatement(Expr * expr, Statement * stmt, int line){
            this->expr = expr;
            this->stmt = stmt;
            this->line = line;
        }
        Expr* expr;
        Statement * stmt;
        int line;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){ return FORS_STATEMENT; }
};

class ForStatement: public Statement{
    public:
        ForStatement(Statement * init, Expr * expr, Statement * inc, Statement * stmt, int line){
            this->expr = expr;
            this->stmt = stmt;
            this->line = line;
            this->init = init;
            this->inc = inc;
        }
        Expr* expr;
        Statement * stmt;
        Statement * init;
        Statement * inc;
        int line;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){ return FOR_STATEMENT; }
};

class ElseStatement : public Statement{
    public:
        ElseStatement(Expr * conditionalExpr, Statement * trueStatement, Statement * falseStatement, int line){
            this->conditionalExpr = conditionalExpr;
            this->trueStatement = trueStatement;
            this->line = line;
            this->falseStatement = falseStatement;
        }
        Expr * conditionalExpr;
        Statement * trueStatement;
        Statement * falseStatement;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){ return ELSE_STATEMENT; }
};

class IfStatement : public Statement{
    public:
        IfStatement(Expr * conditionalExpr, Statement * trueStatement, int line){
            this->conditionalExpr = conditionalExpr;
            this->trueStatement = trueStatement;
            this->line = line;
        }
        Expr * conditionalExpr;
        Statement * trueStatement;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){ return IF_STATEMENT; }
};

class ExprStatement : public Statement{
    public:
        ExprStatement(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){ return EXPRESSION_STATEMENT; }
};

class ReturnStatement : public Statement{
    public:
        ReturnStatement(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){ return RETURN_STATEMENT; }
};

class PrintStatement : public Statement{
    public:
        PrintStatement(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){ return PRINT_STATEMENT; }
};

IMPLEMENT_BINARY_EXPR(Add);
IMPLEMENT_BINARY_EXPR(Sub);
IMPLEMENT_BINARY_EXPR(Mul);
IMPLEMENT_BINARY_EXPR(Div);
IMPLEMENT_BINARY_EXPR(Mod);
IMPLEMENT_BINARY_EXPR(Exp);
IMPLEMENT_BINARY_EXPR(Eq);
IMPLEMENT_BINARY_EXPR(Neq);
IMPLEMENT_BINARY_EXPR(Gte);
IMPLEMENT_BINARY_EXPR(Lte);
IMPLEMENT_BINARY_EXPR(Gt);
IMPLEMENT_BINARY_EXPR(Lt);
IMPLEMENT_BINARY_EXPR(LogicalAnd);
IMPLEMENT_BINARY_EXPR(LogicalOr);
IMPLEMENT_BINARY_EXPR(Assign);
IMPLEMENT_BINARY_EXPR(PlusAssign);
IMPLEMENT_BINARY_EXPR(MinusAssign);
