#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"

extern Asm assemblyFile;

int globalStackPointer = 0;

class VariableInfo{
    public:
        VariableInfo(int offset, bool isArray, bool isParameter, Type type){
            this->offset = offset;
            this->isArray = isArray;
            this->isParameter = isParameter;
            this->type = type;
        }
        int offset;
        bool isArray;
        bool isParameter;
        Type type;
};

map<string, VariableInfo *> codeGenerationVars;

class ContextStack{
    public:
        struct ContextStack* prev;
        map<string, Type> variables;
};

class FunctionInfo{
    public:
        Type returnType;
        list<Parameter *> parameters;
};

int labelCounter = 0;
map<string, Type> globalVariables = {};
map<string, Type> variables;
map<string, FunctionInfo*> methods;
map<string, Type> resultTypes ={
    {"INT,INT", INT},
    {"FLOAT,FLOAT", FLOAT},
    {"INT,FLOAT", FLOAT},
    {"FLOAT,INT", FLOAT},
    {"BOOL,BOOL", BOOL},
    {"STRING,STRING", STRING},
    {"INT_ARRAY,INT_ARRAY", INT_ARRAY},
    {"FLOAT_ARRAY,FLOAT_ARRAY", FLOAT_ARRAY},
    {"INT_ARRAY,FLOAT_ARRAY", FLOAT_ARRAY},
    {"FLOAT_ARRAY,INT_ARRAY", FLOAT_ARRAY},
    {"BOOL_ARRAY,BOOL_ARRAY", BOOL_ARRAY},
    {"STRING_ARRAY,STRING_ARRAY", STRING_ARRAY},
};

map<string, Type> resultTypesAri ={
    {"INT,INT", INT},
    {"FLOAT,FLOAT", FLOAT},
    {"INT,FLOAT", FLOAT},
    {"FLOAT,INT", FLOAT},
    {"STRING,STRING", STRING},
};

const char * intTemps[] = {"$t0","$t1", "$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9" };
const char * floatTemps[] = {"$f0",
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define INT_TEMP_COUNT 10
#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

string getIntTemp(){
    for (int i = 0; i < INT_TEMP_COUNT; i++)
    {
        if(intTempMap.find(intTemps[i]) == intTempMap.end()){
            intTempMap.insert(intTemps[i]);
            return string(intTemps[i]);
        }
    }
    return "";
}

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    return "";
}

void releaseIntTemp(string temp){
    intTempMap.erase(temp);
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

string getTypeName(Type type){
    switch(type){
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case VOID:
            return "VOID";
        case INT_ARRAY:
            return "INT_ARRAY";
        case FLOAT_ARRAY:
            return "FLOAT_ARRAY";
        case BOOL:
            return "BOOL";
        case STRING:
            return "STRING";
        case BOOL_ARRAY:
            return "BOOL_ARRAY";
        case STRING_ARRAY:
            return "STRING_ARRAY";
        case INVALID:
            break;
    }

    cout<<"Unknown type"<< endl;
    exit(0);
}

ContextStack * context = NULL;

void pushContext(){
    variables.clear();
    ContextStack * c = new ContextStack();
    c->variables = variables;
    c->prev = context;
    context = c;
}

void popContext(){
    if(context != NULL){
        ContextStack * previous = context->prev;
        free(context);
        context = previous;
    }
}

string getNewLabel(string prefix){
    stringstream ss;
    ss<<prefix << labelCounter;
    labelCounter++;
    return ss.str();
}

Type getLocalVariableType(string id){
    ContextStack * currContext = context;
    while(currContext != NULL){
        if(currContext->variables[id] != 0)
            return currContext->variables[id];
        currContext = currContext->prev;
    }
    if(!context->variables.empty())
        return context->variables[id];
    return INVALID;
}


Type getVariableType(string id){
    if(!globalVariables.empty())
        return globalVariables[id];
    return INVALID;
}


bool variableExists(string id){
  Type value;
  if(context != NULL){
    value = getLocalVariableType(id);
    //context->variables[id] != 0
    if(value != 0)
      return true;
  }
  return false;
}

int BlockStatement::evaluateSemantic(){
    list<Declaration *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declaration * dec = *itd;
        if(dec != NULL){
            dec->evaluateSemantic();
        }

        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            stmt->evaluateSemantic();
        }

        its++;
    }

    return 0;
}

string BlockStatement::genCode(){
    stringstream ss;

    list<Declaration *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declaration * dec = *itd;
        if(dec != NULL){
            ss<<dec->genCode()<<endl;
        }

        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            ss<<stmt->genCode()<<endl;
        }

        its++;
    }
    return ss.str();
}

int Declaration::evaluateSemantic(){
    list<InitDeclarator * >::iterator it = this->declarations.begin();
    while(it != this->declarations.end()){
        InitDeclarator * declaration = (*it);
        if(declaration->declarator->isArray){
            if(declaration->declarator->arrayDeclaration == NULL && declaration->initializer == NULL){
                cout<<"error: storage size of: "<<declaration->declarator->id  <<" is unknown, line: "<<this->line<<endl;
                exit(0);
            }
        }
        if(declaration->initializer != NULL){
            list<Expr *>::iterator ite = declaration->initializer->expressions.begin();
            while(ite!= declaration->initializer->expressions.end()){
                Type exprType = (*ite)->getType();
                
                if(exprType != FLOAT && exprType != INT){
                    cout<<"EXPRE 2"<< exprType << endl;
                    cout<<"error: invalid conversion from: "<< getTypeName(exprType) <<" to " <<getTypeName(this->type)<< ", line: "<<this->line <<endl;
                    exit(0);
                }
                ite++;
            }
        }
        if(!variableExists(declaration->declarator->id)){
            context->variables[declaration->declarator->id] = this->type;
        }else{
            cout<<"error: redefinition of variable: "<< declaration->declarator->id<< ", line: "<<this->line <<endl;
            exit(0);
        }
    it++;
  }
  return 0;
}

string Declaration::genCode(){
    stringstream code;
    list<InitDeclarator *>::iterator it = this->declarations.begin();
    while(it != this->declarations.end()){
        InitDeclarator * declaration = (*it);
        if (!declaration->declarator->isArray)
        {
           codeGenerationVars[declaration->declarator->id] = new VariableInfo(globalStackPointer, false, false, this->type);
           globalStackPointer +=4;
        }

        if(declaration->initializer != NULL){
            list<Expr *>::iterator itExpr = declaration->initializer->expressions.begin();
            for (int i = 0; i < declaration->initializer->expressions.size(); i++)
            {
                Code exprCode;
                (*itExpr)->genCode(exprCode);
                code << exprCode.code <<endl;
                if(exprCode.type == INT)
                    code << "sw " << exprCode.place <<", "<< codeGenerationVars[declaration->declarator->id]->offset<< "($sp)"<<endl;
                else if(exprCode.type == FLOAT)
                    code << "s.s " << exprCode.place <<", "<< codeGenerationVars[declaration->declarator->id]->offset<< "($sp)"<<endl;

                releaseIntTemp(exprCode.place);
                itExpr++;
            }
            
        }
       it++; 
    }
    return code.str();
}

int GlobalDeclaration::evaluateSemantic(){
    
    return 0;
}

string GlobalDeclaration::genCode(){
    list<InitDeclarator *>::iterator it = this->declaration->declarations.begin();
    stringstream data;
    stringstream code;
    while (it != this->declaration->declarations.end())
    {
        InitDeclarator * declaration = (*it);
        data << declaration->declarator->id <<": .word 0"<<endl;
        if(declaration->initializer != NULL){
            list<Expr *>::iterator itExpr = declaration->initializer->expressions.begin();
            for(int i = 0; i < declaration->initializer->expressions.size(); i++){
                Code exprCode;
                (*itExpr)->genCode(exprCode);
                code << exprCode.code;
                if(!declaration->declarator->isArray){
                    if(exprCode.type == INT)
                        code << "sw "<< exprCode.place<< ", " << declaration->declarator->id<<endl;
                    else if(exprCode.type == FLOAT)
                        code << "s.s "<< exprCode.place<< ", " << declaration->declarator->id<<endl;
                }
                releaseIntTemp(exprCode.place);
                itExpr++;
            }
        }

        it++;
    }

    assemblyFile.data += data.str();
    return code.str();
}


void addMethodDeclaration(string id, int line, Type type, ParameterList params){
    if(methods[id] != 0){
        cout<<"Redefinition of function "<<id<<" in line: "<<line<<endl;
        exit(0);
    }
    methods[id] = new FunctionInfo();
    methods[id]->returnType = type;
    methods[id]->parameters = params;
}

int MethodDefinition::evaluateSemantic(){
    addMethodDeclaration(this->id, this->line, this->type, this->params);
    pushContext();
   
    list<Parameter* >::iterator it = this->params.begin();
    while(it != this->params.end()){
        (*it)->evaluateSemantic();
        it++;
    }

    if(this->statement !=NULL ){
        this->statement->evaluateSemantic();
    }
    
    popContext();

    return 0;
}

string MethodDefinition::genCode(){
    return this->statement->genCode();
}

Type IntExpr::getType(){
    return INT;
}

void IntExpr::genCode(Code &code){
    string temp = getIntTemp();
    code.place = temp;
    stringstream ss;
    ss << "li " << temp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = INT;
}

Type BoolExpr::getType(){
    return BOOL;
}

void BoolExpr::genCode(Code &code){
    string temp = getIntTemp();
    code.place = temp;
    stringstream ss;
    if(this->value)
        ss << "li " << temp <<", "<< 1 <<endl;
    else
        ss << "li " << temp <<", "<< 0 <<endl;
    code.code = ss.str();
    code.type = INT;
}

Type FloatExpr::getType(){
    return FLOAT;
}

void FloatExpr::genCode(Code &code){
    string floatTemp = getFloatTemp();
    code.place = floatTemp;
    stringstream ss;
    ss << "li.s " << floatTemp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = FLOAT;
}

#define IMPLEMENT_BINARY_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypes[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: type "<< leftType <<" can't be converted to type "<< rightType <<", line: "<<this->line<<endl;\
        exit(0);\
    }\
    return resultType; \
}\

void toFloat(Code &code){
    if(code.type == INT){
        string floatTemp = getFloatTemp();
        stringstream ss;
        ss << code.code
        << "mtc1 "<< code.place << ", " << floatTemp <<endl
        << "cvt.s.w " << floatTemp<< ", " << floatTemp <<endl;
        releaseFloatTemp(code.place);
        code.place = floatTemp;
        code.type = FLOAT;
        code.code =  ss.str();
    }
    else{
    }
}

#define IMPLEMENT_BINARY_ARIT_GEN_CODE(name, op)\
void name##Expr::genCode(Code &code){\
    Code leftCode, rightCode;\
    stringstream ss;\
    this->expr1->genCode(leftCode);\
    this->expr2->genCode(rightCode);\
    if(leftCode.type == INT && rightCode.type == INT){\
        code.type = INT;\
        releaseIntTemp(leftCode.place);\
        releaseIntTemp(rightCode.place);\
        ss<< leftCode.code << endl\
        << rightCode.code <<endl\
        << intArithmetic(leftCode, rightCode, code, op)<< endl;\
    }else{\
        code.type = FLOAT;\
        toFloat(leftCode);\
        toFloat(rightCode);\
        releaseIntTemp(leftCode.place);\
        releaseIntTemp(rightCode.place);\
        releaseFloatTemp(leftCode.place);\
        releaseFloatTemp(rightCode.place);\
        ss << leftCode.code << endl\
        << rightCode.code <<endl\
        << floatArithmetic(leftCode, rightCode, code, op)<<endl;\
    }\
    code.code = ss.str();\
}\

#define IMPLEMENT_BINARY_EQUALITY_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypes[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: type "<< leftType <<" can't be converted to type "<< rightType <<", line: "<<this->line<<endl;\
        exit(0);\
    }\
    return BOOL; \
}\

#define IMPLEMENT_BINARY_LOGICALOP_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypes[leftType+","+rightType];\
    if(leftType+","+rightType != "BOOL,BOOL"){\
        cerr<< "Error: Values for Logical Operation must be booleans, line: "<<this->line<<endl;\
        exit(0);\
    }\
    return BOOL; \
}\

#define IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypesAri[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: both values must be arithmetic, line: "<<this->line<<endl;\
        exit(0);\
    }\
    return resultType; \
}\

#define IMPLEMENT_BINARY_ARITH_BOOL_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypesAri[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: both values must be arithmetic, line: "<<this->line<<endl;\
        exit(0);\
    }\
    return BOOL; \
}\


string intArithmetic(Code &leftCode, Code &rightCode, Code &code, char op){
    stringstream ss;
    code.place = getIntTemp();
    switch (op)
    {
        case '+':
            ss << "add "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '-':
            ss << "sub "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '*':
            ss << "mult "<< leftCode.place <<", "<< rightCode.place <<endl
            << "mflo "<< code.place;
            break;
        case '/':
            ss << "div "<< leftCode.place <<", "<< rightCode.place <<endl
            << "mflo "<< code.place;
            break;
        case '%':
            ss << "div "<< leftCode.place <<", "<< rightCode.place <<endl
            << "mfhi "<< code.place;
            break;
        default:
            break;
    }
    return ss.str();
}

string floatArithmetic(Code &leftCode, Code &rightCode, Code &code, char op){
    stringstream ss;
    code.place = getFloatTemp();
    switch (op)
    {
        case '+':
            ss << "add.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '-':
            ss << "sub.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '*':
            ss << "mul.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '/':
            ss << "div.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        default:
            break;
    }
    return ss.str();
}

Type getUnaryType(Type expressionType, int unaryOperation){
    switch(unaryOperation){
        case INCREMENT:
        case DECREMENT:
            if(expressionType == INT || expressionType == FLOAT)
                return expressionType;
        case NOT:
            if(expressionType == BOOL)
                return BOOL;
    }

    cerr<<"Error: Invalid type"<<endl;
    exit(0);
}

int Parameter::evaluateSemantic(){
    if(!variableExists(this->declarator->id)){
        context->variables[declarator->id] = this->type;
    }else{
        cout<<"error: redefinition of variable: "<< declarator->id<< ", line: "<<this->line <<endl;
        exit(0);
    }
    return 0;
}


Type Value::getType(){
    Type value;
    if(context != NULL){
        value = getLocalVariableType(this->id);
        if(value != 0)
            return value;
    }
    value = getVariableType(this->id);
    if(value == 0){
        cout<<"error: '"<<this->id<<"' was not declared in this scope, line: "<<this->line<<endl;
        exit(0);
    }
    return value;
}

void Value::genCode(Code &code){
}

int CreateVariableStatement::evaluateSemantic(){
    ValueList::iterator ite = this->valueList.begin();
    ArgumentList::iterator arg = this->args.begin();
    while(ite!= this->valueList.end()){
        Value * lvalue = (*ite);
        

        if (this->args.size() == 0)
        {
            if(!variableExists(lvalue->id)){
                if(lvalue->expr != NULL){
                    if(this->type == STRING)
                        context->variables[lvalue->id] = STRING_ARRAY;
                    if(this->type == INT)
                        context->variables[lvalue->id] = INT_ARRAY;
                    if(this->type == BOOL)
                        context->variables[lvalue->id] = BOOL_ARRAY;
                    if(this->type == FLOAT)
                        context->variables[lvalue->id] = FLOAT_ARRAY;
                }else{
                    context->variables[lvalue->id] = this->type;
                }
            }else{
                cout<<"error: redefinition of variable: "<< lvalue->id << ", line: "<<this->line <<endl;
                exit(0);
            }

            return 0;
        }
        
        Expr * expr = (*arg);
        auto exprType = getTypeName(expr->getType());
        
        if(this->type != INVALID){
            
            Type type = this->type;

            if(lvalue->expr != NULL){
                if(this->type == STRING)
                    type = STRING_ARRAY;
                if(this->type == INT)
                    type = INT_ARRAY;
                if(this->type == BOOL)
                    type = BOOL_ARRAY;
                if(this->type == FLOAT)
                    type = FLOAT_ARRAY;
            }
            auto lvalueType = getTypeName(type);
            Type resultType = resultTypes[lvalueType+","+exprType];

            if(resultType == 0){
                cerr<< "Error: type "<< lvalueType <<" can't be converted to type"<< exprType <<", line: "<<this->line<<endl;
                exit(0);
            }

            if(!variableExists(lvalue->id)){
                context->variables[lvalue->id] = type; 
            }else{
                cout<<"error: redefinition of variable: "<< lvalue->id << ", line: "<<this->line <<endl;
                exit(0);
            }
        }else{
            if(!variableExists(lvalue->id)){
                context->variables[lvalue->id] = expr->getType(); 
            }else{
                cout<<"error: redefinition of variable: "<< lvalue->id << ", line: "<<this->line <<endl;
                exit(0);
            }
        }

        ite++;
        arg++;
    }
    return 0;
}

string CreateVariableStatement::genCode(){
   return ""; 
}

int AssignmentStatement::evaluateSemantic(){
    ValueList::iterator ite = this->valueList.begin();
    ArgumentList::iterator arg = this->args.begin();

    while(ite!= this->valueList.end()){
        Value * lvalue = (*ite);
        Expr * expr = (*arg);

        auto exprType = getTypeName(expr->getType());
        auto lvalueType = getTypeName(lvalue->getType());

        if (lvalue->expr!=NULL )
        {
            if(lvalue->getType() == STRING_ARRAY)
                lvalueType = getTypeName(STRING);
            if(lvalue->getType() == INT_ARRAY)
                lvalueType = getTypeName(INT);
            if(lvalue->getType() == BOOL_ARRAY)
                lvalueType = getTypeName(BOOL);
            if(lvalue->getType() == FLOAT_ARRAY)
                lvalueType = getTypeName(FLOAT);
        }
        
        Type resultType = resultTypes[lvalueType+","+exprType];
        if(resultType == 0){
            cerr<< "Error: type "<< lvalueType <<" can't be converted to type "<< exprType <<", line: "<<this->line<<endl;
            exit(0);
        }

        ite++;
        arg++;
    }
    
    return 0;
}

string AssignmentStatement::genCode(){
   return ""; 
}

int BreakStatement::evaluateSemantic(){
    return 0;
}

string BreakStatement::genCode(){
   return ""; 
}

int ContinueStatement::evaluateSemantic(){
    return 0;
}

string ContinueStatement::genCode(){
   return ""; 
}

int ForStatement::evaluateSemantic(){
    pushContext();
    if (this->init != NULL)
    {
        this->init->evaluateSemantic();
    }

    this->expr->getType();

    if(this->stmt != NULL){
        this->stmt->evaluateSemantic();
    }
   
    if (this->inc != NULL)
    {
        this->inc->evaluateSemantic();
    }
    
    popContext();
    
    return 0;
}

string ForStatement::genCode(){
   return ""; 
}

Type UnaryExpr::getType(){
    Type exprType = this->expr->getType();
    return getUnaryType(exprType, this->type);
}

void UnaryExpr::genCode(Code &code){
    
}

Type ArrayExpr::getType(){
    return this->id->getType();
}

void ArrayExpr::genCode(Code &code){
    
}

Type IdExpr::getType(){
    Type value;
    if(context != NULL){
        value = getLocalVariableType(this->id);
        if(value != 0)
            return value;
    }
    value = getVariableType(this->id);
    if(value == 0){
        cout<<"error: '"<<this->id<<"' was not declared in this scope, line: "<<this->line<<endl;
        exit(0);
    }
    return value;
}

Type ArrayInitExpr::getType(){
    ArgumentList::iterator arg = this->args.begin();
    while(arg!= this->args.end()){
        Expr * lvalue = (*arg);
        string argType = getTypeName(lvalue->getType());
        string arrayType = getTypeName(this->type);
        if (lvalue->getType() != this->type){
            cerr<< "Error: type "<< argType <<" can't be converted to array type "<< arrayType <<", line: "<<this->line<<endl;
            exit(0);
        }

        arg++;
    }
    if(this->type == STRING){
        return STRING_ARRAY;
    }
    if(this->type == INT){
        return INT_ARRAY;
    }
    if(this->type == BOOL){
        return BOOL_ARRAY;
    }
    if(this->type == FLOAT){
        return FLOAT_ARRAY;
    }
    return INVALID;
}

void IdExpr::genCode(Code &code){
    if(codeGenerationVars.find(this->id) == codeGenerationVars.end()){
        code.type = globalVariables[this->id];
        if(globalVariables[this->id] == INT){
            string intTemp = getIntTemp();
            code.place = intTemp;
            code.code = "lw "+ intTemp + ", "+ this->id + "\n";
        }else if(globalVariables[this->id] == FLOAT){
            string floatTemp = getFloatTemp();
            code.place = floatTemp;
            code.code = "l.s "+ floatTemp + ", "+ this->id + "\n";
        }
    }
   else{
        code.type = codeGenerationVars[this->id]->type;
        if(codeGenerationVars[this->id]->type == INT && !codeGenerationVars[this->id]->isArray){
            string intTemp = getIntTemp();
            code.place = intTemp;
            code.code = "lw "+ intTemp + ", " + to_string(codeGenerationVars[this->id]->offset) +"($sp)\n";
        }else if(codeGenerationVars[this->id]->type == FLOAT && !codeGenerationVars[this->id]->isArray){
            string floatTemp = getFloatTemp();
            code.place = floatTemp;
            code.code = "l.s "+ floatTemp + ", " +to_string(codeGenerationVars[this->id]->offset) +"($sp)\n";
        }
    }
}

Type MethodInvocationExpr::getType(){
    if (this->id->id == "fmt.Println" || this->id->id == "rand"){
        /* code */
        list<Expr *>::iterator argsIt =this->args.begin();
        while(argsIt != this->args.end()){
            string argType = getTypeName((*argsIt)->getType());
            
            argsIt++;
        }
        return VOID;
    }
    
    // cout<<"CHECK 1"<<endl;
    FunctionInfo * func = methods[this->id->id];
    if(func == NULL){
        cout<<"error: function "<<this->id->id<<" not found, line: "<<this->line<<endl;
        exit(0);
    }
    // cout<<"CHECK 2"<<endl;
    Type funcType = func->returnType;
    if(func->parameters.size() > this->args.size()){
        cout<<"error: to few arguments to function "<<this->id->id<<", line: "<<this->line<<endl;
        exit(0);
    }
    // cout<<"CHECK 3"<<endl;
    if(func->parameters.size() < this->args.size()){
        cout<<"error: to many arguments to function "<<this->id->id<<", line: "<<this->line<<endl;
        exit(0);
    }
    // cout<<"CHECK 4"<<endl;
    list<Parameter *>::iterator paramIt = func->parameters.begin();
    list<Expr *>::iterator argsIt =this->args.begin();
    while(paramIt != func->parameters.end() && argsIt != this->args.end()){
        string paramType = getTypeName((*paramIt)->type);
        string argType = getTypeName((*argsIt)->getType());
        if( paramType != argType){
            cout<<"error: invalid conversion from: "<< argType <<" to " <<paramType<< ", line: "<<this->line <<endl;
            exit(0);
        }
        paramIt++;
        argsIt++;
    }

    return funcType;
}

void MethodInvocationExpr::genCode(Code &code){
    
}

Type PostIncrementExpr::getType(){
    return this->expr->getType();
}

void PostIncrementExpr::genCode(Code &code){
    
}

void ArrayInitExpr::genCode(Code &code){
    
}

Type PostDecrementExpr::getType(){
    return this->expr->getType();
}

void PostDecrementExpr::genCode(Code &code){

}

Type StringExpr::getType(){
    return STRING;
}

void StringExpr::genCode(Code &code){
    string strLabel = getNewLabel("string");
    stringstream ss;
    ss << strLabel <<": \"" << this->value << "\""<<endl;
    assemblyFile.data += ss.str(); 
    code.code = "";
    code.place = strLabel;
}

int ForsStatement::evaluateSemantic(){
    if(this->expr->getType() != BOOL){
        cout<<"Expression for while must be boolean, line "<< this->line << endl;
        exit(0);
    }
    
    pushContext();
    if(this->stmt != NULL){
        this->stmt->evaluateSemantic();
    }
    popContext();
    return 0;
}

string ForsStatement::genCode(){
    stringstream ss;
    string forsLabel = getNewLabel("for");
    string endForsLabel = getNewLabel("endFor");
    Code code;
    this->expr->genCode(code);
    ss << forsLabel << ": "<< endl
    << code.code <<endl
    << "beqz "<< code.place << ", " << endForsLabel <<endl
    << this->stmt->genCode() <<endl
    << "j " << forsLabel <<endl
    << endForsLabel << ": "<<endl;
    return ss.str();
}

int ElseStatement::evaluateSemantic(){
    if(this->conditionalExpr->getType() != BOOL){
        cout<<"Expression for if must be boolean";
        exit(0);
    }
    pushContext();
    this->trueStatement->evaluateSemantic();
    popContext();
    pushContext();
    if(this->falseStatement != NULL)
        this->falseStatement->evaluateSemantic();
    popContext();
    return 0;
}

string ElseStatement::genCode(){
    string elseLabel = getNewLabel("else");
    string endIfLabel = getNewLabel("endif");
    Code exprCode;
    this->conditionalExpr->genCode(exprCode);
    stringstream code;
    code << exprCode.code << endl
    << "beqz "<< exprCode.place << ", " << elseLabel <<endl
    << this->trueStatement->genCode() << endl
    << "j " <<endIfLabel << endl
    << elseLabel <<": " <<endl
    << this->falseStatement->genCode() <<endl
    << endIfLabel <<" :"<< endl;
    releaseIntTemp(exprCode.place);
    return code.str();
}

int IfStatement::evaluateSemantic(){
    if(this->conditionalExpr->getType() != BOOL){
        cout<<"Expression for if must be boolean";
        exit(0);
    }
    pushContext();
    this->trueStatement->evaluateSemantic();
    popContext();
    return 0;
}

string IfStatement::genCode(){
    string endIfLabel = getNewLabel("endif");
    Code exprCode;
    this->conditionalExpr->genCode(exprCode);
    stringstream code;
    code << exprCode.code << endl
    << "beqz "<< exprCode.place << ", " << endIfLabel <<endl
    << this->trueStatement->genCode() << endl
    << endIfLabel <<" :"<< endl;
    releaseIntTemp(exprCode.place);
    return code.str();
}

int ExprStatement::evaluateSemantic(){
    return this->expr->getType();
}

string ExprStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    return exprCode.code;
}

int ReturnStatement::evaluateSemantic(){
    return this->expr->getType();
}

string ReturnStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    if(this->expr->getType() == INT){
        releaseIntTemp(exprCode.place);
    }else{
        releaseFloatTemp(exprCode.place);
    }

    stringstream ss;
    ss << exprCode.code << endl
    << "move $v0, "<< exprCode.place <<endl;
    return ss.str();
}

int PrintStatement::evaluateSemantic(){
    return this->expr->getType();
}

string PrintStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    stringstream code;
    code<< exprCode.code<<endl;
    if(exprCode.type == INT){
        code <<"move $a0, "<< exprCode.place<<endl
        << "li $v0, 1"<<endl
        << "syscall"<<endl;
    }else if(exprCode.type == FLOAT){
        code << "mov.s $f12, "<< exprCode.place<<endl
        << "li $v0, 2"<<endl
        << "syscall"<<endl;
    }
    return code.str();
}

void EqExpr::genCode(Code &code){

}

void NeqExpr::genCode(Code &code){
    
}

void GteExpr::genCode(Code &code){
    
}

void LteExpr::genCode(Code &code){
    
}

void GtExpr::genCode(Code &code){
    
}

void LtExpr::genCode(Code &code){
    
}

void LogicalAndExpr::genCode(Code &code){
    
}

void LogicalOrExpr::genCode(Code &code){
    
}

void AssignExpr::genCode(Code &code){
    
}
void PlusAssignExpr::genCode(Code &code){
    
}

void MinusAssignExpr::genCode(Code &code){
    
}

IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Add);
IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Sub);
IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Mul);
IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Div);
IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Mod);
IMPLEMENT_BINARY_ARITHMETIC_GET_TYPE(Exp);
IMPLEMENT_BINARY_GET_TYPE(Assign);
IMPLEMENT_BINARY_GET_TYPE(PlusAssign);
IMPLEMENT_BINARY_GET_TYPE(MinusAssign);

IMPLEMENT_BINARY_ARIT_GEN_CODE(Add, '+');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Sub, '-');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Mul, '*');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Div, '/');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Mod, '%');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Exp, '^');

IMPLEMENT_BINARY_EQUALITY_GET_TYPE(Eq);
IMPLEMENT_BINARY_EQUALITY_GET_TYPE(Neq);
IMPLEMENT_BINARY_ARITH_BOOL_GET_TYPE(Gte);
IMPLEMENT_BINARY_ARITH_BOOL_GET_TYPE(Lte);
IMPLEMENT_BINARY_ARITH_BOOL_GET_TYPE(Gt);
IMPLEMENT_BINARY_ARITH_BOOL_GET_TYPE(Lt);
IMPLEMENT_BINARY_LOGICALOP_GET_TYPE(LogicalAnd);
IMPLEMENT_BINARY_LOGICALOP_GET_TYPE(LogicalOr);
