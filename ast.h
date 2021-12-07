#include <string>
#include <list>
#include <map>

using namespace std;

class Statement; 

typedef list<Statement *> StatementList;
enum StatementKind
{
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
    ELSE_STATEMENT
};

enum UnaryType
{
    INCREMENT,
    DECREMENT,
    NOT
};

class Statement{
    public:
        int line;
        virtual int evaluateSemantic() = 0;
        virtual StatementKind getKind() = 0;
};

class Expr{
    public:
        int line;
        virtual Type getType() = 0;
};

#define IMPLEMENT_BINARY_EXPR(name)\
class name##Expr : public BinaryExpr{\
    public:\
        name##Expr(Expr * expr1, Expr *expr, int line) : BinaryExpr(expr1, expr2, line){}\
        Type getType();\


};

class Declarator{
    public
        Declarator(string id, Expr* arrayDeclaration, bool isArray, int line){
            this->id = id;
            this->arrayDeclaration = arrayDeclaration;
            this-> isArray = isArray;
            this->line = line;
        }
        string id;
        Expr *arrayDeclaration;
        bool isArray;
        int line;
};

class BlockStatement : public Statement{
    public:
        BlockStatement(StatementList statements, int line ){

        }
        StatementList statement;
        int line;
        int evaluateSemantic();
        StatementKind getKind(){
            return BLOCK_STATEMENT;
        }
};

IMPLEMENT_BINARY_EXPR(Add);
IMPLEMENT_BINARY_EXPR(Sub);
IMPLEMENT_BINARY_EXPR(Mul);
IMPLEMENT_BINARY_EXPR(Div);
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