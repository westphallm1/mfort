/* via http://epaperpress.com/lexandyacc/download/LexAndYaccTutorial.pdf
 * by Tom Niemann
 */
#ifndef fortran63_h
#define fortran63_h
typedef enum { typeInt, typeFloat, typeIntFn, typeFloatFn, typeTag} type_t; 
/* sym table entry */
typedef struct SymTableEntry {
    type_t type;
    int common; //which common value is it? or none
    int isfunc;
    union {
        int currdim;//counter for current dim, make sure dims are correct
        int currarg;//counter for current arg, make sure args are correct
    };
    union {
        int ndim;
        int nargs;
    };
    union {
        int dimensions[3]; //support up to 3d
        type_t argtypes[99]; //support up to 99 args
    };
    char * key;
    struct SymTableEntry * next; //for symtable
    struct SymTableEntry * equiv; //for aliasing

} sym;

/* constants */ 
typedef struct { 
    int value;                  /* value of constant */ 
} intNodeType; 

typedef struct { 
    float value;                  /* value of constant */ 
} floatNodeType; 

/* identifiers */ 
typedef struct { 
    char * s;                      /* subscript to sym array */ 
} strNodeType; 
/* operators */ 
typedef struct { 
    int oper;                   /* operator */ 
    int nops;                   /* number of operands */ 
    struct nodeTypeTag **op;  /* operands, extended at runtime 
*/ 
} oprNodeType; 

typedef struct nodeTypeTag { 
    oprNodeType opr;        /* operators */ 
    type_t castTo;
    sym * symVal;
    int lineno; /* for errors in name analysis/typechecking */
    int charno; /* for errors in name analysis/typechecking */
    union { 
        floatNodeType fVal;        /* constants */ 
        intNodeType iVal;        /* constants */ 
        strNodeType sVal;
    }; 
} nodeType; 


#endif
