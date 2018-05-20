#include "fortran63.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>


int main(){
    symTable * myTable = newSymTable(113);
    addLocal(myTable,"foo");
    addLocal(myTable,"bar");
    addLocal(myTable,"baz");
    addLocal(myTable,"qux");
    addScope(myTable);
    addLocal(myTable,"Foo");
    addLocal(myTable,"Bar");
    addLocal(myTable,"Baz");
    addLocal(myTable,"Qux");
    addScope(myTable);
    addLocal(myTable,"FOO");
    addLocal(myTable,"BAR");
    addLocal(myTable,"BAZ");
    addLocal(myTable,"QUX");
    printTable(myTable);
    removeScope(myTable);
    printTable(myTable);
}
