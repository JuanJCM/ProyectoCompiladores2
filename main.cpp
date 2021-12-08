#include <stdio.h>
#include <string>

using namespace std;

extern FILE * yyin;
extern char* yytext;
int yyparse();

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "File not found %s\n", argv[0]);
        return 1;
    }

    FILE * f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        return 1;
    }

    yyin = f;

    yyparse();
    return 0;
}
