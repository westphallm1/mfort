/* Figure out the number of dimensions of dimensioned variables and number of
 * arguments to functions. Assign Common values to Common variables and set
 * equivalences
 */
#include "fortran63.h"
#include "fortran63.tab.h" 
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum{addSyms, setDims, checkDims, setArgs, setCommon,
             setEquiv, setExternArgs, checkArgC} action_t;
/* Macros for common access chains */
#define child(idx) node->opr.op[idx]
#define hasChild(idx) node->opr.op[idx]!=NULL
#define childOp(idx) node->opr.op[idx]->opr.oper
#define nodeName() node -> sVal.s
#define callChild(idx) jmptable[node->opr.op[idx]->opr.oper]\
    (node->opr.op[idx],act,curr)
FILE * OUT;
symTable * TABLE;
int COMMON_COUNT = 0;
int FOUND_ERROR = 0;
char * DIM_ACCESS = "Invalid expression in subscript.";

extern nodeType * yyrootptr;

sym * intlit_(nodeType *,action_t,sym *);
sym * floatlit_(nodeType *,action_t,sym *);
sym * tag_(nodeType *,action_t,sym *);
sym * intid_(nodeType *,action_t,sym *);
sym * floatid_(nodeType *,action_t,sym *);
sym * intfnid_(nodeType *,action_t,sym *);
sym * floatfnid_(nodeType *,action_t,sym *);
sym * floatfmtlit_(nodeType *,action_t,sym *);
sym * expfmtlit_(nodeType *,action_t,sym *);
sym * intfmtlit_(nodeType *,action_t,sym *);
sym * holfmtlit_(nodeType *,action_t,sym *);
sym * assign_(nodeType *,action_t,sym *);
sym * call_(nodeType *,action_t,sym *);
sym * comma_(nodeType *,action_t,sym *);
sym * common_(nodeType *,action_t,sym *);
sym * continue_(nodeType *,action_t,sym *);
sym * dimension_(nodeType *,action_t,sym *);
sym * do_(nodeType *,action_t,sym *);
sym * end_(nodeType *,action_t,sym *);
sym * equivalence_(nodeType *,action_t,sym *);
sym * format_(nodeType *,action_t,sym *);
sym * function_(nodeType *,action_t,sym *);
sym * goto_(nodeType *,action_t,sym *);
sym * if_(nodeType *,action_t,sym *);
sym * lparen_(nodeType *,action_t,sym *);
sym * newline_(nodeType *,action_t,sym *);
sym * pause_(nodeType *,action_t,sym *);
sym * print_(nodeType *,action_t,sym *);
sym * read_(nodeType *,action_t,sym *);
sym * return_(nodeType *,action_t,sym *);
sym * rparen_(nodeType *,action_t,sym *);
sym * stop_(nodeType *,action_t,sym *);
sym * subprocess_(nodeType *,action_t,sym *);
sym * plus_(nodeType *,action_t,sym *);
sym * minus_(nodeType *,action_t,sym *);
sym * times_(nodeType *,action_t,sym *);
sym * divide_(nodeType *,action_t,sym *);
sym * pow_(nodeType *,action_t,sym *);
sym * uminus_(nodeType *,action_t,sym *);
sym * stmt_(nodeType *,action_t,sym *);
sym * stmtlist_(nodeType *,action_t,sym *);
sym * fnassign_(nodeType *,action_t,sym *);
sym * fncall_(nodeType *,action_t,sym *);
sym * indexed_(nodeType *,action_t,sym *);
sym * formallist_(nodeType *,action_t,sym *);
sym * actuallist_(nodeType *,action_t,sym *);
sym * labellist_(nodeType *,action_t,sym *);
sym * fmtlist_(nodeType *,action_t,sym *);
sym * loclist_(nodeType *,action_t,sym *);
sym * dimlist_(nodeType *,action_t,sym *);
sym * explist_(nodeType *,action_t,sym *);
sym * program_(nodeType *,action_t,sym *);
sym * main_(nodeType *,action_t,sym *);

sym * (*jmptable[])(nodeType *,action_t,sym *) = {
    [INTLIT] = intlit_,
    [FLOATLIT] = floatlit_,
    [TAG] = tag_,
    [INTID] = intid_,
    [FLOATID] = floatid_,
    [INTFNID] = intfnid_,
    [FLOATFNID] = floatfnid_,
    [FLOATFMTLIT] = floatfmtlit_,
    [EXPFMTLIT] = expfmtlit_,
    [INTFMTLIT] = intfmtlit_,
    [HOLFMTLIT] = holfmtlit_,
    [ASSIGN] = assign_,
    [CALL] = call_,
    [COMMA] = comma_,
    [COMMON] = common_,
    [CONTINUE] = continue_,
    [DIMENSION] = dimension_,
    [DO] = do_,
    [END] = end_,
    [EQUIVALENCE] = equivalence_,
    [FORMAT] = format_,
    [FUNCTION] = function_,
    [GOTO] = goto_,
    [IF] = if_,
    [LPAREN] = lparen_,
    [NEWLINE] = newline_,
    [PAUSE] = pause_,
    [PRINT] = print_,
    [READ] = read_,
    [RETURN] = return_,
    [RPAREN] = rparen_,
    [STOP] = stop_,
    [SUBPROCESS] = subprocess_,
    [PLUS] = plus_,
    [MINUS] = minus_,
    [TIMES] = times_,
    [DIVIDE] = divide_,
    [POW] = pow_,
    [UMINUS] = uminus_,
    [STMT] = stmt_,
    [STMTLIST] = stmtlist_,
    [FNASSIGN] = fnassign_,
    [FNCALL] = fncall_,
    [INDEXED] = indexed_,
    [FORMALLIST] = formallist_,
    [ACTUALLIST] = actuallist_,
    [LABELLIST] = labellist_,
    [FMTLIST] = fmtlist_,
    [LOCLIST] = loclist_,
    [DIMLIST] = dimlist_,
    [EXPLIST] = explist_,
    [PROGRAM] = program_,
    [MAIN] = main_,
};

void error(nodeType * node, char * msg){
    while(node->lineno == -1)
        node = child(0);
    printf("%d,%d: %s\n",node->lineno,node->charno,msg);
    printErrorLine(node->lineno,node->charno);
    FOUND_ERROR = 1;
}

sym * intlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            if(curr->ndim == 3)
                error(node,"Maximum subscript dimensions exceeded");
            curr->dimensions[curr->ndim++]=node->iVal.value;
            break;
        case checkDims:
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeFloat; //autocast
            break;
    }
    return NULL;
}
sym * floatlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeFloat;
            break;
    }
    return NULL;
}

sym * tag_(nodeType * node, action_t act, sym * curr){
    if(node -> iVal.value == -1){
        return NULL;
    }else{
        char * key = malloc(10*sizeof(char));
        snprintf(key,10,"%d",node->iVal.value);
        if((curr = getLocal(TABLE,key)) != NULL){
            free(key);
        }else{
            curr = addLocal(TABLE,key);
            curr -> type = typeTag;
        }
    }
    return curr;
}
sym * intid_(nodeType * node, action_t act, sym * curr){
    sym * equiv = NULL;
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case setArgs:
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeInt;
            break;
        case setEquiv:
            equiv = curr;
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeInt;
    }
    if(equiv != NULL)
        curr -> equiv = equiv;
    if(act == setCommon)
        curr->common=COMMON_COUNT++;
    return curr;
}
sym * floatid_(nodeType * node, action_t act, sym * curr){
    sym * equiv = NULL;
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setArgs:
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeFloat;
            break;
        case setEquiv:
            equiv = curr;
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeFloat;
    }
    if(equiv != NULL)
        curr -> equiv = equiv;
    if(act == setCommon)
        curr->common=++COMMON_COUNT;
    return curr;
}
sym * intfnid_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeInt; //autocast
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeIntFn;
        curr -> isfunc = 1;
    }
    return curr;
}
sym * floatfnid_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeFloat; //autocast
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeFloatFn;
        curr -> isfunc = 1;
    }
    return curr;
}
sym * floatfmtlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
    }
    return NULL;
}
sym * expfmtlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
    }
    return NULL;
}
sym * intfmtlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
    }
    return NULL;
}
sym * holfmtlit_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
    }
    return NULL;
}

sym * assign_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * call_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * comma_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * common_(nodeType * node, action_t act, sym * curr){
    act = setCommon;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * continue_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * dimension_(nodeType * node, action_t act, sym * curr){
    act = setDims;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * do_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    if(hasChild(4)){
        callChild(4);
    }
    return NULL;
}
sym * end_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * equivalence_(nodeType * node, action_t act, sym * curr){
    act = setEquiv;
    curr = NULL;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * format_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * function_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) curr = callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    return NULL;
}
sym * goto_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)){
        callChild(0);
    }
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * if_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    return NULL;
}
sym * lparen_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * newline_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * pause_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * print_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * read_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * return_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * rparen_(nodeType * node, action_t act, sym * curr){
    return NULL;
}
sym * stop_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * subprocess_(nodeType * node, action_t act, sym * curr){
    
    if(hasChild(0))callChild(0);
    
    if(hasChild(1))callChild(1);
    
    if(hasChild(2))callChild(2);

    return NULL;
}
sym * plus_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            if((childOp(0) != TIMES && childOp(0) != INTID) || 
                    childOp(1) != INTLIT)
                error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    
    return NULL;
}
sym * minus_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            if((childOp(0) != TIMES && childOp(0) != INTID) || 
                    childOp(1) != INTLIT)
                error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * times_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            if(childOp(0) != INTLIT || childOp(1) != INTID)
                error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * divide_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(child(0),"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * pow_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(child(0),"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * uminus_(nodeType * node, action_t act, sym * curr){
    switch(act){
        case setDims:
            error(child(0),"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(child(0),DIM_ACCESS);
            break;
    }
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * stmt_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * stmtlist_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * fnassign_(nodeType * node, action_t act, sym * curr){
    act = addSyms;
    if(hasChild(0)) curr = callChild(0);
    curr -> nargs = 0;
    act = setArgs;
    addScope(TABLE);
    if(hasChild(1)) callChild(1);
    act = addSyms;
    if(hasChild(2)) callChild(2);
    removeScope(TABLE);
    return NULL;
}
sym * fncall_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) curr = callChild(0);
    act = checkArgC;
    curr -> currarg = 1;
    
    if(hasChild(1)) callChild(1);
    if(curr->currarg != curr->nargs)
        error(child(0),"Wrong number of function arguments.");
    
    return NULL;
}
sym * indexed_(nodeType * node, action_t act, sym * curr){
    action_t tmp = act;
    if(act == setExternArgs)
        curr->nargs++;
    act = addSyms;
    if(hasChild(0)) curr = callChild(0);
    act = tmp;
    if(act != setDims){ //dimSetting is a given, otherwise figure it out
        if(curr->ndim > 0 && !curr->isfunc){
            act = checkDims;
            curr->currdim = 1;
        }else if(curr->isfunc){
            act = checkArgC;
            curr -> currarg = 1;
            //check args in typechecking
        } else {
            //it's treated as a function, assume it's defined elsewhere
            curr->isfunc = 1;
            act = setExternArgs;
        }
    }
    if(hasChild(1)) callChild(1);
    //make sure that we have the right number of dims
    if(act == checkDims){
        if(curr->currdim != curr->ndim)
            error(child(0),"Wrong number of indices in subscript access.");
        curr->currdim = 0;
    }
    if(act == checkArgC){
        if(curr->currarg != curr->nargs)
            error(child(0),"Inconsistent argument count to external function.");
        curr->currarg = 0;
    }
    return NULL;
}
sym * formallist_(nodeType * node, action_t act, sym * curr){
    if(act == setEquiv && curr == NULL){
        act = addSyms;
        curr = callChild(0);
        act = setEquiv;
    } else {
        callChild(0);
    }
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * actuallist_(nodeType * node, action_t act, sym * curr){
    if(act == checkArgC)
        curr->currarg++;
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * labellist_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * fmtlist_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * loclist_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * dimlist_(nodeType * node, action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * explist_(nodeType * node, action_t act, sym * curr){
    if(act == checkDims)
        curr->currdim++;
    if(act == checkArgC)
        curr->currarg++;
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}

sym * main_(nodeType * node, action_t act, sym * curr){
    callChild(0);
    return NULL;
}

sym * program_(nodeType * node, action_t act, sym * curr){
    callChild(0);
    return NULL;
}


void nameanalysis(){
    TABLE = newSymTable(113);
    program_(yyrootptr,addSyms,NULL);
}
