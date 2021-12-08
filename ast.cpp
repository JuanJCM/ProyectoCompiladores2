#include "ast.h"
#include <iostream>

class Context{
    public:
        struct ContextStack* prev;
        map<string, Type> variables;
};
