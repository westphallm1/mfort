#include "fortran63.h"
#include "fortran63.tab.h" 
#include <stdio.h>

#define child(idx) node->opr.op[idx]
#define hasChild(idx) node->opr.op[idx]!=NULL
#define childOp(idx) node->opr.op[idx]->opr.oper
#define callChild(idx) jmptable[node->opr.op[idx]->opr.oper](node->opr.op[idx])
FILE * out;
extern nodeType * yyrootptr;

void intlit_(nodeType *);
void floatlit_(nodeType *);
void tag_(nodeType *);
void intid_(nodeType *);
void floatid_(nodeType *);
void intfnid_(nodeType *);
void floatfnid_(nodeType *);
void floatfmtlit_(nodeType *);
void expfmtlit_(nodeType *);
void intfmtlit_(nodeType *);
void holfmtlit_(nodeType *);
void assign_(nodeType *);
void call_(nodeType *);
void comma_(nodeType *);
void common_(nodeType *);
void continue_(nodeType *);
void dimension_(nodeType *);
void do_(nodeType *);
void end_(nodeType *);
void equivalence_(nodeType *);
void format_(nodeType *);
void function_(nodeType *);
void goto_(nodeType *);
void if_(nodeType *);
void lparen_(nodeType *);
void newline_(nodeType *);
void pause_(nodeType *);
void print_(nodeType *);
void read_(nodeType *);
void return_(nodeType *);
void rparen_(nodeType *);
void stop_(nodeType *);
void subprocess_(nodeType *);
void plus_(nodeType *);
void minus_(nodeType *);
void times_(nodeType *);
void divide_(nodeType *);
void pow_(nodeType *);
void uminus_(nodeType *);
void stmt_(nodeType *);
void stmtlist_(nodeType *);
void fnassign_(nodeType *);
void fncall_(nodeType *);
void indexed_(nodeType *);
void formallist_(nodeType *);
void actuallist_(nodeType *);
void labellist_(nodeType *);
void fmtlist_(nodeType *);
void loclist_(nodeType *);
void dimlist_(nodeType *);
void explist_(nodeType *);
void program_(nodeType *);
void main_(nodeType *);

void (*jmptable[])(nodeType *) = {
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

void intlit_(nodeType * node){
    printf("%d",node -> iVal.value);

}
void floatlit_(nodeType * node){
    printf("%10.4f",node -> fVal.value);
}

void tag_(nodeType * node){
    if(node -> iVal.value == -1){
        printf("      ");
    }else{
        printf("%5d ",node -> iVal.value);
    }
}
void intid_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void floatid_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void intfnid_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void floatfnid_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void floatfmtlit_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void expfmtlit_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void intfmtlit_(nodeType * node){
    printf("%s",node -> sVal.s);

}
void holfmtlit_(nodeType * node){
    printf("%s",node -> sVal.s);

}

void assign_(nodeType * node){
    if(hasChild(0))callChild(0);
    printf(" = ");
    if(hasChild(1))callChild(1);

}
void call_(nodeType * node){
    printf("CALL ");
    if(hasChild(0))callChild(0);
    printf("(");
    if(hasChild(1))callChild(1);
    printf(")");

}
void comma_(nodeType * node){

}
void common_(nodeType * node){
    printf("COMMON (");
    if(hasChild(0))callChild(0);
    printf(")");

}
void continue_(nodeType * node){
    printf("CONTINUE");

}
void dimension_(nodeType * node){
    printf("DIMENSION (");
    if(hasChild(0))callChild(0);
    printf(")");

}
void do_(nodeType * node){
    printf("DO ");
    if(hasChild(0))callChild(0);
    printf(" ");
    if(hasChild(1))callChild(1);
    printf(" = ");
    if(hasChild(2))callChild(2);
    printf(", ");
    if(hasChild(3))callChild(3);
    if(hasChild(4)){
        printf(", ");
        callChild(4);
    }
}
void end_(nodeType * node){
    printf("      END\n");

}
void equivalence_(nodeType * node){
    printf("EQUIVALENCE (");
    if(hasChild(0))callChild(0);
    printf(")");

}
void format_(nodeType * node){
    printf("FORMAT (");
    if(hasChild(0))callChild(0);
    printf(")");

}
void function_(nodeType * node){
    printf("FUNCTION ");
    if(hasChild(0))callChild(0);
    printf("(");
    if(hasChild(1))callChild(1);
    printf(")");
    if(hasChild(2))callChild(2);

}
void goto_(nodeType * node){
    printf("GO TO ");
    if(hasChild(0)){
        printf("(");
        callChild(0);
        printf("), ");
    }
    if(hasChild(1)) callChild(1);

}
void if_(nodeType * node){

    printf("IF (");
    if(hasChild(0))callChild(0);
    printf(") ");
    if(hasChild(1))callChild(1);
    printf(",");
    if(hasChild(2))callChild(2);
    printf(",");
    if(hasChild(3))callChild(3);
}
void lparen_(nodeType * node){

}
void newline_(nodeType * node){
    printf("\n");

}
void pause_(nodeType * node){
    printf("PAUSE");
    if(hasChild(0))callChild(0);

}
void print_(nodeType * node){
    printf("PRINT ");
    if(hasChild(0))callChild(0);
    printf(", ");
    if(hasChild(1))callChild(1);

}
void read_(nodeType * node){
    printf("READ ");
    if(hasChild(0))callChild(0);
    printf(", ");
    if(hasChild(1))callChild(1);

}
void return_(nodeType * node){
    printf("RETURN");
}
void rparen_(nodeType * node){

}
void stop_(nodeType * node){
    printf("STOP ");
    if(hasChild(0))callChild(0);
}
void subprocess_(nodeType * node){
    printf("SUBPROCESS ");
    if(hasChild(0))callChild(0);
    printf("(");
    if(hasChild(1))callChild(1);
    printf(")");
    if(hasChild(2))callChild(2);

}
void plus_(nodeType * node){
    printf("(");
    if(hasChild(0))callChild(0);
    printf(" + ");
    if(hasChild(1))callChild(1);
    printf(")");
}
void minus_(nodeType * node){
    printf("(");
    if(hasChild(0))callChild(0);
    printf(" - ");
    if(hasChild(1))callChild(1);
    printf(")");

}
void times_(nodeType * node){
    printf("(");
    if(hasChild(0))callChild(0);
    printf(" * ");
    if(hasChild(1))callChild(1);
    printf(")");

}
void divide_(nodeType * node){
    printf("(");
    if(hasChild(0))callChild(0);
    printf(" / ");
    if(hasChild(1))callChild(1);
    printf(")");

}
void pow_(nodeType * node){
    printf("(");
    if(hasChild(0))callChild(0);
    printf(" ** ");
    if(hasChild(1))callChild(1);
    printf(")");

}
void uminus_(nodeType * node){
    printf("-(");
    if(hasChild(0))callChild(0);
    printf(")");

}
void stmt_(nodeType * node){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
    printf("\n");

}
void stmtlist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    if(hasChild(1)) callChild(1);
}
void fnassign_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf("(");
    if(hasChild(1)) callChild(1);
    printf(") = ");
    if(hasChild(2)) callChild(2);

}
void fncall_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf("(");
    if(hasChild(1)) callChild(1);
    printf(")");
}
void indexed_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf("(");
    if(hasChild(1)) callChild(1);
    printf(")");
}
void formallist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void actuallist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void labellist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void fmtlist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void loclist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void dimlist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);

}
void explist_(nodeType * node){
    if(hasChild(0)) callChild(0);
    printf(", ");
    if(hasChild(1)) callChild(1);
   
}

void main_(nodeType * node){
    callChild(0);
}
void program_(nodeType * node){
    callChild(0);
}


void unparse(){
    program_(yyrootptr);
}

int main(){
    yyparse(); 
    unparse();
}
