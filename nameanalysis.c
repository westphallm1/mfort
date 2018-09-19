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
             setEquiv, setExternArgs, checkArgC} na_action_t;
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

sym * na_intlit_(nodeType *,na_action_t,sym *);
sym * na_floatlit_(nodeType *,na_action_t,sym *);
sym * na_tag_(nodeType *,na_action_t,sym *);
sym * na_intid_(nodeType *,na_action_t,sym *);
sym * na_floatid_(nodeType *,na_action_t,sym *);
sym * na_intfnid_(nodeType *,na_action_t,sym *);
sym * na_floatfnid_(nodeType *,na_action_t,sym *);
sym * na_floatfmtlit_(nodeType *,na_action_t,sym *);
sym * na_expfmtlit_(nodeType *,na_action_t,sym *);
sym * na_intfmtlit_(nodeType *,na_action_t,sym *);
sym * na_holfmtlit_(nodeType *,na_action_t,sym *);
sym * na_assign_(nodeType *,na_action_t,sym *);
sym * na_call_(nodeType *,na_action_t,sym *);
sym * na_comma_(nodeType *,na_action_t,sym *);
sym * na_common_(nodeType *,na_action_t,sym *);
sym * na_continue_(nodeType *,na_action_t,sym *);
sym * na_dimension_(nodeType *,na_action_t,sym *);
sym * na_do_(nodeType *,na_action_t,sym *);
sym * na_end_(nodeType *,na_action_t,sym *);
sym * na_equivalence_(nodeType *,na_action_t,sym *);
sym * na_format_(nodeType *,na_action_t,sym *);
sym * na_function_(nodeType *,na_action_t,sym *);
sym * na_goto_(nodeType *,na_action_t,sym *);
sym * na_if_(nodeType *,na_action_t,sym *);
sym * na_lparen_(nodeType *,na_action_t,sym *);
sym * na_newline_(nodeType *,na_action_t,sym *);
sym * na_pause_(nodeType *,na_action_t,sym *);
sym * na_print_(nodeType *,na_action_t,sym *);
sym * na_read_(nodeType *,na_action_t,sym *);
sym * na_return_(nodeType *,na_action_t,sym *);
sym * na_rparen_(nodeType *,na_action_t,sym *);
sym * na_stop_(nodeType *,na_action_t,sym *);
sym * na_subprocess_(nodeType *,na_action_t,sym *);
sym * na_plus_(nodeType *,na_action_t,sym *);
sym * na_minus_(nodeType *,na_action_t,sym *);
sym * na_times_(nodeType *,na_action_t,sym *);
sym * na_divide_(nodeType *,na_action_t,sym *);
sym * na_pow_(nodeType *,na_action_t,sym *);
sym * na_uminus_(nodeType *,na_action_t,sym *);
sym * na_stmt_(nodeType *,na_action_t,sym *);
sym * na_stmtlist_(nodeType *,na_action_t,sym *);
sym * na_fnassign_(nodeType *,na_action_t,sym *);
sym * na_fncall_(nodeType *,na_action_t,sym *);
sym * na_indexed_(nodeType *,na_action_t,sym *);
sym * na_formallist_(nodeType *,na_action_t,sym *);
sym * na_actuallist_(nodeType *,na_action_t,sym *);
sym * na_labellist_(nodeType *,na_action_t,sym *);
sym * na_fmtlist_(nodeType *,na_action_t,sym *);
sym * na_loclist_(nodeType *,na_action_t,sym *);
sym * na_dimlist_(nodeType *,na_action_t,sym *);
sym * na_explist_(nodeType *,na_action_t,sym *);
sym * na_program_(nodeType *,na_action_t,sym *);
sym * na_main_(nodeType *,na_action_t,sym *);

sym * (*jmptable[])(nodeType *,na_action_t,sym *) = {
    [INTLIT] = na_intlit_,
    [FLOATLIT] = na_floatlit_,
    [TAG] = na_tag_,
    [INTID] = na_intid_,
    [FLOATID] = na_floatid_,
    [INTFNID] = na_intfnid_,
    [FLOATFNID] = na_floatfnid_,
    [FLOATFMTLIT] = na_floatfmtlit_,
    [EXPFMTLIT] = na_expfmtlit_,
    [INTFMTLIT] = na_intfmtlit_,
    [HOLFMTLIT] = na_holfmtlit_,
    [ASSIGN] = na_assign_,
    [CALL] = na_call_,
    [COMMA] = na_comma_,
    [COMMON] = na_common_,
    [CONTINUE] = na_continue_,
    [DIMENSION] = na_dimension_,
    [DO] = na_do_,
    [END] = na_end_,
    [EQUIVALENCE] = na_equivalence_,
    [FORMAT] = na_format_,
    [FUNCTION] = na_function_,
    [GOTO] = na_goto_,
    [IF] = na_if_,
    [LPAREN] = na_lparen_,
    [NEWLINE] = na_newline_,
    [PAUSE] = na_pause_,
    [PRINT] = na_print_,
    [READ] = na_read_,
    [RETURN] = na_return_,
    [RPAREN] = na_rparen_,
    [STOP] = na_stop_,
    [SUBPROCESS] = na_subprocess_,
    [PLUS] = na_plus_,
    [MINUS] = na_minus_,
    [TIMES] = na_times_,
    [DIVIDE] = na_divide_,
    [POW] = na_pow_,
    [UMINUS] = na_uminus_,
    [STMT] = na_stmt_,
    [STMTLIST] = na_stmtlist_,
    [FNASSIGN] = na_fnassign_,
    [FNCALL] = na_fncall_,
    [INDEXED] = na_indexed_,
    [FORMALLIST] = na_formallist_,
    [ACTUALLIST] = na_actuallist_,
    [LABELLIST] = na_labellist_,
    [FMTLIST] = na_fmtlist_,
    [LOCLIST] = na_loclist_,
    [DIMLIST] = na_dimlist_,
    [EXPLIST] = na_explist_,
    [PROGRAM] = na_program_,
    [MAIN] = na_main_,
};

void error(nodeType * node, char * msg){
    while(node->lineno == -1)
        node = child(0);
    printf("%d,%d: %s\n",node->lineno,node->charno,msg);
    printErrorLine(node->lineno,node->charno);
    FOUND_ERROR = 1;
}

sym * na_intlit_(nodeType * node, na_action_t act, sym * curr){
    switch(act){
        case setDims:
            if(curr->ndim == 3)
                error(node,"Maximum subscript dimensions exceeded");
            curr->dimensions[curr->ndim++]=node->iVal.value;
            break;
        case checkDims:
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeInt; //don't cast
            break;
    }
    return NULL;
}
sym * na_floatlit_(nodeType * node, na_action_t act, sym * curr){
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

sym * na_tag_(nodeType * node, na_action_t act, sym * curr){
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
            curr -> istag = 1;
        }
    }
    return curr;
}
sym * na_intid_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_floatid_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_intfnid_(nodeType * node, na_action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeInt; //don't cast
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeIntFn;
        curr -> isfunc = 1;
    }
    return curr;
}
sym * na_floatfnid_(nodeType * node, na_action_t act, sym * curr){
    switch(act){
        case setDims:
            error(node,"Dimension declarations must be int literals");
            break;
        case checkDims:
            error(node,"Dimension access must be int literal or int id");
            break;
        case setExternArgs:
            curr->argtypes[curr->nargs++] = typeFloat; //don't cast
            break;
    }
    if((curr = getLocal(TABLE,nodeName())) == NULL){
        curr = addLocal(TABLE,nodeName());
        curr -> type = typeFloatFn;
        curr -> isfunc = 1;
    }
    return curr;
}
sym * na_floatfmtlit_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_expfmtlit_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_intfmtlit_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_holfmtlit_(nodeType * node, na_action_t act, sym * curr){
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

sym * na_assign_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * na_call_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) curr = callChild(0);
    if(curr->issubproc){
        act = checkArgC;
        curr->currarg=1;
    }else if (curr->isfunc){
        error(child(0),"Cannot call a function.");
        return NULL;
    }else if (curr->ndim > 0){
        error(child(0),"Cannot call a subscripted variable.");
        return NULL;
    }else{
        act = setExternArgs;
        curr->issubproc = 1;
    }
    if(hasChild(1))callChild(1);

    if(act == checkArgC && curr->currarg != curr->nargs)
        error(child(0),"Inconsistent argument count to external subprocess.");

    return NULL;
}
sym * na_comma_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_common_(nodeType * node, na_action_t act, sym * curr){
    act = setCommon;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * na_continue_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_dimension_(nodeType * node, na_action_t act, sym * curr){
    act = setDims;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * na_do_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    if(hasChild(4)){
        callChild(4);
    }
    return NULL;
}
sym * na_end_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_equivalence_(nodeType * node, na_action_t act, sym * curr){
    act = setEquiv;
    curr = NULL;
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * na_format_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_function_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) curr = callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    return NULL;
}
sym * na_goto_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)){
        callChild(0);
    }
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_if_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    return NULL;
}
sym * na_lparen_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_newline_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_pause_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * na_print_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * na_read_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    return NULL;
}
sym * na_return_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_rparen_(nodeType * node, na_action_t act, sym * curr){
    return NULL;
}
sym * na_stop_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0))callChild(0);
    return NULL;
}
sym * na_subprocess_(nodeType * node, na_action_t act, sym * curr){
    
    if(hasChild(0))callChild(0);
    
    if(hasChild(1))callChild(1);
    
    if(hasChild(2))callChild(2);

    return NULL;
}
sym * na_plus_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_minus_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_times_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_divide_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_pow_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_uminus_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_stmt_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_stmtlist_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_fnassign_(nodeType * node, na_action_t act, sym * curr){
    act = addSyms;
    if(hasChild(0)) curr = callChild(0);
    curr -> nargs = 0;
    curr->isdeclared = 1;
    act = setArgs;
    addScope(TABLE);
    if(hasChild(1)) callChild(1);
    act = addSyms;
    if(hasChild(2)) callChild(2);
    removeScope(TABLE);
    return NULL;
}

sym * na_fncall_(nodeType * node, na_action_t act, sym * curr){
    if(act == setExternArgs){
        //set arg type
        curr->nargs++;
    }
    if(hasChild(0)) curr = callChild(0);
    act = checkArgC;
    curr -> currarg = 1;
    
    if(hasChild(1)) callChild(1);
    if(curr->currarg != curr->nargs)
        error(child(0),"Wrong number of function arguments.");
    
    return NULL;
}

sym * na_indexed_(nodeType * node, na_action_t act, sym * curr){
    na_action_t tmp = act;
    if(act!=setExternArgs) act = addSyms;
    if(hasChild(0)) curr = callChild(0);
    act = tmp;
    if(act == setDims){
        //raise an eror if it's previously declared 
        if(curr->isdeclared){
            error(child(0),
                "Cannot assign dimensions to previously declared variable.");
            return NULL;
        } else {
            curr->isdeclared = 1;
        }
    } else { //dimSetting is a given, otherwise figure it out
        if(curr->isfunc){
            act = checkArgC;
            curr -> currarg = 1;
        } else if (curr->issubproc){
            error(child(0),
                "Cannot invoke subprocess outside of \"CALL\" statment.");
        } else if(curr->ndim > 0){
            act = checkDims;
            curr->currdim = 1;
        } else {
            //it's treated as a function, assume it's defined elsewhere
            curr->isfunc = 1;
            curr->isdeclared = 1;
            act = setExternArgs;
        }
    }
    if(hasChild(1)) callChild(1);
    //make sure that we have the right number of dims
    if(act == checkDims && curr->currdim != curr->ndim)
        error(child(0),"Wrong number of indices in subscript access.");
    if(act == checkArgC && curr->currarg != curr->nargs)
        error(child(0),"Inconsistent argument count to external function.");

    return NULL;
}
sym * na_formallist_(nodeType * node, na_action_t act, sym * curr){
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
sym * na_actuallist_(nodeType * node, na_action_t act, sym * curr){
    if(act == checkArgC)
        curr->currarg++;
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_labellist_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_fmtlist_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_loclist_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_dimlist_(nodeType * node, na_action_t act, sym * curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}
sym * na_explist_(nodeType * node, na_action_t act, sym * curr){
    if(act == checkDims)
        curr->currdim++;
    if(act == checkArgC)
        curr->currarg++;
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    return NULL;
}

sym * na_main_(nodeType * node, na_action_t act, sym * curr){
    callChild(0);
    return NULL;
}

sym * na_program_(nodeType * node, na_action_t act, sym * curr){
    callChild(0);
    return NULL;
}


void nameanalysis(){
    TABLE = newSymTable(113);
    na_program_(yyrootptr,addSyms,NULL);
}
