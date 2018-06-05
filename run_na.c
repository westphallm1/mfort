#include "fortran63.h"
#include "fortran63.tab.h" 
#include "symtable.h"
#include "nameanalysis.h"

int main(){
    yyparse(); 
    nameanalysis();
    printTable(TABLE);
}

