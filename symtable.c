#include "fortran63.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define currScope(table) table->scopes[table->nscopes]
/* djb2 - via http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void printSym(sym * s){
    printf("  %s\n",s->key);
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
};

void removeScope(symTable *table){
    //keep symbols in tact, only remove the table itself
    free(table->scopes[table->nscopes]);
    table->nscopes--;
};


sym * addLocal(symTable *table, char * key){

    sym * new_sym = malloc(sizeof(sym));
    //set default features, let the caller change appropriately
    new_sym -> ndim = 0;
    new_sym -> currdim = 0;
    new_sym -> nargs= 0;
    new_sym -> isfunc= 0;
    new_sym -> common = -1;
    new_sym -> key = key;
    new_sym -> next = NULL;
    new_sym -> equiv= NULL;

    int idx = hash(key)%table->scope_size;
    if(currScope(table)[idx] == 0){
        currScope(table)[idx] = new_sym;
    }else{
        sym * curr = currScope(table)[idx];
        while(curr->next != NULL)
            curr = curr->next;
        curr->next = new_sym;
    }
    return new_sym;
}

sym * getLocal(symTable *table, char * key){
    int idx = hash(key)%table->scope_size;
    if(currScope(table)[idx] == 0){
        return NULL;
    }else{
        sym * curr = currScope(table)[idx];
        while(curr->next != NULL && strcmp(key,curr->key))
            curr = curr->next;
        if(!strcmp(key,curr->key))
            return curr;
        else
            return NULL;
    }

}

