#include "fortran63.h"
#include "fortran63.tab.h" 
#include <stdio.h>

extern int yylex();

int main(){
    int c;
    while((c=yylex())> 1){
        switch(c) {
            case INTLIT: printf("INTLIT "); break;
            case FLOATLIT: printf("FLOATLIT "); break;
            case TAG: printf("TAG "); break;
            case INTID: printf("INTID "); break;
            case FLOATID: printf("FLOATID "); break;
            case INTFNID: printf("INTFNID "); break;
            case FLOATFNID: printf("FLOATFNID "); break;
            case FLOATFMTLIT: printf("FLOATFMTLIT "); break;
            case EXPFMTLIT: printf("EXPFMTLIT "); break;
            case INTFMTLIT: printf("INTFMTLIT "); break;
            case HOLFMTLIT: printf("HOLFMTLIT "); break;
            case ASSIGN: printf("ASSIGN "); break;
            case CALL: printf("CALL "); break;
            case COMMA: printf("COMMA "); break;
            case COMMON: printf("COMMON "); break;
            case CONTINUE: printf("CONTINUE "); break;
            case DIMENSION: printf("DIMENSION "); break;
            case DO: printf("DO "); break;
            case END: printf("END "); break;
            case EQUIVALENCE: printf("EQUIVALENCE "); break;
            case FORMAT: printf("FORMAT "); break;
            case FUNCTION: printf("FUNCTION "); break;
            case GOTO: printf("GOTO "); break;
            case IF: printf("IF "); break;
            case LPAREN: printf("LPAREN "); break;
            case NEWLINE: printf("NEWLINE "); break;
            case PAUSE: printf("PAUSE "); break;
            case PRINT: printf("PRINT "); break;
            case READ: printf("READ "); break;
            case RETURN: printf("RETURN "); break;
            case RPAREN: printf("RPAREN "); break;
            case STOP: printf("STOP "); break;
            case SUBPROCESS: printf("SUBPROCESS "); break;
            case PLUS: printf("PLUS "); break;
            case MINUS: printf("MINUS "); break;
            case TIMES: printf("TIMES "); break;
            case DIVIDE: printf("DIVIDE "); break;
            case POW: printf("POW "); break;
            case UMINUS: printf("UMINUS "); break;
            case STMT: printf("STMT "); break;
            case STMTLIST: printf("STMTLIST "); break;
            case FNASSIGN: printf("FNASSIGN "); break;
            case FNCALL: printf("FNCALL "); break;
            case INDEXED: printf("INDEXED "); break;
            case FORMALLIST: printf("FORMALLIST "); break;
            case ACTUALLIST: printf("ACTUALLIST "); break;
            case LABELLIST: printf("LABELLIST "); break;
            case FMTLIST: printf("FMTLIST "); break;
            case LOCLIST: printf("LOCLIST "); break;
            case DIMLIST: printf("DIMLIST "); break;
            case EXPLIST: printf("EXPLIST "); break;
            case PROGRAM: printf("PROGRAM "); break;
            case MAIN: printf("MAIN "); break;
        }
    }
}
