/* 
 * Ensure that the values on the left and right-hand sides of assignments
 * match types, check for consistency in function arguments, and figure out
 * which statements require casting from int to float (float->int is disallowed)
 */
#include "fortran63.h"
#include "fortran63.tab.h" 
#include "symtable.h"
#include "nameanalysis.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum{setCastType, setSymToCheck, checkArgType} ta_action_t;
typedef union ta_return_value {
    type_t t;
    sym *  s;
} ta_ret_t;

ta_ret_t NO_RET;
/* Macros for common access chains */
#define child(idx) node->opr.op[idx]
#define hasChild(idx) node->opr.op[idx]!=NULL
#define childOp(idx) node->opr.op[idx]->opr.oper
#define nodeName() node -> sVal.s
#define callChild(idx) ta_jmptable[node->opr.op[idx]->opr.oper]\
    (node->opr.op[idx],act,curr,s_curr)
FILE * OUT;
symTable * TABLE;

extern nodeType * yyrootptr;

ta_ret_t ta_intlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_floatlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_tag_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_intid_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_floatid_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_intfnid_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_floatfnid_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_floatfmtlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_expfmtlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_intfmtlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_holfmtlit_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_assign_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_call_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_comma_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_common_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_continue_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_dimension_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_do_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_end_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_equivalence_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_format_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_function_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_goto_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_if_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_lparen_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_newline_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_pause_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_print_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_read_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_return_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_rparen_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_stop_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_subprocess_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_plus_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_minus_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_times_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_divide_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_pow_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_uminus_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_stmt_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_stmtlist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_fnassign_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_fncall_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_indexed_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_formallist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_actuallist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_labellist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_fmtlist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_loclist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_dimlist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_explist_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_program_(nodeType *,ta_action_t,type_t,sym *);
ta_ret_t ta_main_(nodeType *,ta_action_t,type_t,sym *);

ta_ret_t (*ta_jmptable[])(nodeType *,ta_action_t,type_t,sym *) = {
    [INTLIT] = ta_intlit_,
    [FLOATLIT] = ta_floatlit_,
    [TAG] = ta_tag_,
    [INTID] = ta_intid_,
    [FLOATID] = ta_floatid_,
    [INTFNID] = ta_intfnid_,
    [FLOATFNID] = ta_floatfnid_,
    [FLOATFMTLIT] = ta_floatfmtlit_,
    [EXPFMTLIT] = ta_expfmtlit_,
    [INTFMTLIT] = ta_intfmtlit_,
    [HOLFMTLIT] = ta_holfmtlit_,
    [ASSIGN] = ta_assign_,
    [CALL] = ta_call_,
    [COMMA] = ta_comma_,
    [COMMON] = ta_common_,
    [CONTINUE] = ta_continue_,
    [DIMENSION] = ta_dimension_,
    [DO] = ta_do_,
    [END] = ta_end_,
    [EQUIVALENCE] = ta_equivalence_,
    [FORMAT] = ta_format_,
    [FUNCTION] = ta_function_,
    [GOTO] = ta_goto_,
    [IF] = ta_if_,
    [LPAREN] = ta_lparen_,
    [NEWLINE] = ta_newline_,
    [PAUSE] = ta_pause_,
    [PRINT] = ta_print_,
    [READ] = ta_read_,
    [RETURN] = ta_return_,
    [RPAREN] = ta_rparen_,
    [STOP] = ta_stop_,
    [SUBPROCESS] = ta_subprocess_,
    [PLUS] = ta_plus_,
    [MINUS] = ta_minus_,
    [TIMES] = ta_times_,
    [DIVIDE] = ta_divide_,
    [POW] = ta_pow_,
    [UMINUS] = ta_uminus_,
    [STMT] = ta_stmt_,
    [STMTLIST] = ta_stmtlist_,
    [FNASSIGN] = ta_fnassign_,
    [FNCALL] = ta_fncall_,
    [INDEXED] = ta_indexed_,
    [FORMALLIST] = ta_formallist_,
    [ACTUALLIST] = ta_actuallist_,
    [LABELLIST] = ta_labellist_,
    [FMTLIST] = ta_fmtlist_,
    [LOCLIST] = ta_loclist_,
    [DIMLIST] = ta_dimlist_,
    [EXPLIST] = ta_explist_,
    [PROGRAM] = ta_program_,
    [MAIN] = ta_main_,
};

ta_ret_t ta_intlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeFloat && act == setCastType)
        node -> castTo = curr;
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_floatlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeInt && act == setCastType)
        error(node,"Can't cast float to int.");
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_tag_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_intid_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeFloat && act == setCastType)
        node -> castTo = curr;
    if(act == setSymToCheck){
        ta_ret_t r = {.s = getLocal(TABLE,nodeName())};
        return r;
    }
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_floatid_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeInt && act == setCastType)
        error(node,"Can't cast float to int.");
    if(act == setSymToCheck){
        ta_ret_t r = {.s = getLocal(TABLE,nodeName())};
        return r;
    }
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_intfnid_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeFloat && act == setCastType)
        node -> castTo = curr;
    if(act == setSymToCheck){
        ta_ret_t r = {.s = getLocal(TABLE,nodeName())};
        return r;
    }
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_floatfnid_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(curr == typeInt && act == setCastType)
        error(node,"Can't cast float to int.");
    if(act == setSymToCheck){
        ta_ret_t r = {.s = getLocal(TABLE,nodeName())};
        return r;
    }
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_floatfmtlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_expfmtlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_intfmtlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_holfmtlit_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_assign_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    curr = typeNone;
    act = setCastType;
    //set the cast type to the LHS of the assign
    if(hasChild(0)) curr = callChild(0).t;
    //cast everything on the RHS 
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_call_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    act = checkArgType;
    if(hasChild(0)) s_curr = callChild(0).s;
    if(hasChild(1)) curr = callChild(1).t;
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_comma_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_common_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_continue_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_dimension_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_do_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    if(hasChild(4)){
        callChild(4);
    }
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_end_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_equivalence_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_format_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_function_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_goto_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)){
        callChild(0);
    }
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_if_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    if(hasChild(2))callChild(2);
    if(hasChild(3))callChild(3);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_lparen_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_newline_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_pause_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_print_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_read_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_return_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_rparen_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_stop_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_subprocess_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    
    if(hasChild(0))callChild(0);
    
    if(hasChild(1))callChild(1);
    
    if(hasChild(2))callChild(2);

    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_plus_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_minus_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_times_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_divide_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_pow_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    if(hasChild(1))callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_uminus_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0))callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_stmt_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_stmtlist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_fnassign_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_fncall_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_indexed_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_formallist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_actuallist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_labellist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_fmtlist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_loclist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_dimlist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}
ta_ret_t ta_explist_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_main_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}

ta_ret_t ta_program_(nodeType * node, ta_action_t act, type_t curr, sym * s_curr){
    callChild(0);
    ta_ret_t r = {.t = typeNone};
    return r;
}

void typeanalysis(){
    ta_program_(yyrootptr,setCastType,typeNone,NULL);
}
