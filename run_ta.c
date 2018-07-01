#include "fortran63.h"
#include "fortran63.tab.h" 
#include "symtable.h"
#include "nameanalysis.h"
#include "typeanalysis.h"

int main(int argc, char * argv[]){
    setInput(fopen(argv[1],"r"));
    yyparse(); 
    nameanalysis();
    typeanalysis();
    printTable(TABLE);
}

