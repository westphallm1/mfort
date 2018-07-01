#include "fortran63.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CURR_SCOPE(table) table->scopes[table->nscopes]
/* djb2 - via http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
/*
 * Set of debugging functions to verify syms are correctly constructed
 */
void printDims(sym * s){
    printf(" (");
    printf("%d",s->dimensions[0]);   
    for(int i = 1; i < s->ndim; i++){
        printf(", %d",s->dimensions[i]);
    }
    printf(")");
}

void printArg(type_t arg){
    switch(arg){
        case typeInt:
            printf("%s","Int");break;
        case typeFloat:
            printf("%s","Float");break;
        case typeIntFn:
            printf("%s","IntFn");break;
        case typeFloatFn:
            printf("%s","FloatFn");break;
        case typeTag:
            printf("%s","Tag");break;
    }
}
void printArgs(sym * s){
    printf(" (");
    printArg(s->argtypes[0]);
    for(int i = 1; i < s->nargs; i++){
        printf(", ");
        printArg(s->argtypes[i]);
    }
    printf(")");

}
void printSym(sym * s){
    printf("  %s",s->key);
    if(s->isfunc){
        printArgs(s);
        printf(" (function)");
    }else if(s->issubproc){
        printArgs(s);
        printf(" (subprocess)");
    }else if (s->istag){
        printf(" (tag)");
    }else if(s->ndim > 0) {
        printDims(s);
    }
    if(s->common > -1){
        printf(" (common%d)", s->common);
    }
    if(s->equiv != NULL){
        printf(" ( equivalent %s)",s->equiv->key);
    }
    printf("\n");
}

void printTable(symTable * table){
    for(int i = 0; i <= table->nscopes; i++){
        printf("--Scope %d--\n",i);
        for(int j = 0; j < table->scope_size; j++){
            if(table->scopes[i][j] == 0)
                continue;
            sym * curr = table->scopes[i][j];
            while(curr != NULL){
                printSym(curr);
                curr = curr->next;
            }
        }
    }
}

symTable * newSymTable(int size){
    symTable * table = malloc(sizeof(symTable));
    table->scope_size = size;
    table->nscopes = 0;
    //start everything null
    table->scopes[0] = calloc(size,sizeof(sym **));

    return table;
}

void addScope(symTable *table){
    table->nscopes++;
    table->scopes[table->nscopes] = calloc(table->scope_size,sizeof(sym **));
}

void removeScope(symTable *table){
    //keep symbols in tact, only remove the table itself
    free(table->scopes[table->nscopes]);
    table->nscopes--;
}


sym * addLocal(symTable *table, char * key){
    sym * new_sym = malloc(sizeof(sym));
    //set default features, let the caller change appropriately
    new_sym -> ndim = 0;
    new_sym -> currdim = 0;
    new_sym -> nargs = 0;
    new_sym -> isfunc = 0;
    new_sym -> issubproc = 0;
    new_sym -> istag = 0;
    new_sym -> isdeclared = 0;
    new_sym -> common = -1;
    new_sym -> cast_to = typeNone;
    new_sym -> key = key;
    new_sym -> next = NULL;
    new_sym -> equiv= NULL;

    int idx = hash(key)%table->scope_size;
    if(CURR_SCOPE(table)[idx] == 0){
        CURR_SCOPE(table)[idx] = new_sym;
    }else{
        sym * curr = CURR_SCOPE(table)[idx];
        while(curr->next != NULL)
            curr = curr->next;
        curr->next = new_sym;
    }
    return new_sym;
}

sym * getLocal(symTable *table, char * key){
    int idx = hash(key)%table->scope_size;
    if(CURR_SCOPE(table)[idx] == 0){
        return NULL;
    }else{
        sym * curr = CURR_SCOPE(table)[idx];
        while(curr->next != NULL && strcmp(key,curr->key))
            curr = curr->next;
        if(!strcmp(key,curr->key))
            return curr;
        else
            return NULL;
    }
}

