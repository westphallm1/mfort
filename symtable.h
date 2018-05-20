#ifndef symtable_h
#define symtable_h

typedef struct {
    int scope_size;
    int nscopes;
    int offset;
    sym ** scopes[10];

} symTable;

symTable * newSymTable(int size);
void printTable(symTable *);
void addScope(symTable *);
void removeScope(symTable *);
sym * addLocal(symTable *, char * key);
sym * getLocal(symTable *, char * key);

#endif
