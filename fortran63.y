%{ 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdarg.h> 
#include <string.h> 
#include "fortran63.h" 
/* prototypes */ 
nodeType *opr(int oper, int nops, ...); 
nodeType *id(struct LeafNode value, int type); 
nodeType *intCon(struct LeafNode value); 
nodeType *floatCon(struct LeafNode value); 
nodeType *tagCon(struct LeafNode value); 

#define INT_ID(x) id(x, INTID)
#define FLOAT_ID(x) id(x, FLOATID)
#define INT_FN_ID(x) id(x, INTFNID)
#define FLOAT_FN_ID(x) id(x, FLOATFNID)
#define intFmtId(x) id(x, INTFMTLIT)
#define floatFmtId(x) id(x, FLOATFMTLIT)
#define expFmtId(x) id(x, EXPFMTLIT)
#define holFmtId(x) id(x, HOLFMTLIT)
#define setLocation() p->lineno = value.lineno; p->charno = value.charno;

void freeNode(nodeType *p); 
int yylex(void); 
void yyerror(char *s); 
extern int yylineno;
extern int yycharno;

nodeType * yyrootptr;

%} 
%union { 
    struct LeafNode leafNode;
    nodeType *nPtr;             /* node pointer */ 
}; 
%define parse.error verbose
%token <leafNode> INTLIT
%token <leafNode> FLOATLIT
%token <leafNode> TAG
%token <leafNode> INTID
%token <leafNode> FLOATID
%token <leafNode> INTFNID
%token <leafNode> FLOATFNID
%token <leafNode> FLOATFMTLIT
%token <leafNode> EXPFMTLIT
%token <leafNode> INTFMTLIT
%token <leafNode> HOLFMTLIT
%token ASSIGN CALL COMMA COMMON CONTINUE DIMENSION DO END 
%token EQUIVALENCE FORMAT FUNCTION GOTO 
%token IF LPAREN NEWLINE PAUSE PRINT READ RETURN RPAREN STOP SUBPROCESS
%left PLUS MINUS
%left TIMES DIVIDE
%left POW
%nonassoc UMINUS 
/*Just for unique values to opr*/
%token STMT STMTLIST FNASSIGN FNCALL INDEXED FORMALLIST ACTUALLIST
%token LABELLIST FMTLIST LOCLIST DIMLIST EXPLIST PROGRAM MAIN
/*nonterminals*/
%type <nPtr>actualList assignStmt commonStmt continueStmt dimList dimStmt
%type <nPtr>doStmt equivStmt exp fmt fmtStmt fmtList fnAssignStmt 
%type <nPtr>fnCallExp fnId fnProgDecl formalList gotoStmt id ifStmt
%type <nPtr>indexed expList intVal labelList loc locList
%type <nPtr>mainStmts pauseStmt printStmt readStmt returnStmt
%type <nPtr>stmt stmtBlock stmtList stopStmt subCallStmt subProcDecl
%type <nPtr>term varGotoStmt
%% 

program:
    mainStmts { yyrootptr = $1; }
    | fnProgDecl { yyrootptr = $1; }
    | subProcDecl { yyrootptr = $1; }
    ;

mainStmts:
    stmtList { $$ = opr(MAIN, 1, $1); }
    ;

fnProgDecl:
    FUNCTION id LPAREN formalList RPAREN NEWLINE stmtList {
          $$ = opr(FUNCTION, 3, $2, $4, $7); }
    ;

subProcDecl:
    SUBPROCESS id LPAREN formalList RPAREN NEWLINE stmtList { 
          $$ = opr(SUBPROCESS, 3, $2, $4, $7); }
    | SUBPROCESS id LPAREN RPAREN NEWLINE stmtList {
          $$ = opr(SUBPROCESS, 3, $2, NULL, $6); }
    ;

stmtList:
    stmtBlock stmtList { $$ = opr(STMTLIST,2,$1, $2); }
    | stmtBlock { $$ = $1; }
    ;

stmtBlock:
    TAG stmt NEWLINE { $$ = opr(STMT,2,tagCon($1),$2); }
    | TAG END NEWLINE { $$ = opr(END,2, tagCon($1), NULL); }
    | NEWLINE {$$ = NULL;}
    ;

stmt:
    assignStmt { $$ = $1; }
    | ifStmt { $$ = $1; }
    | gotoStmt { $$ = $1; }
    | varGotoStmt { $$ = $1; }
    | doStmt { $$ = $1; }
    | fmtStmt { $$ = $1; }
    | printStmt { $$ = $1; }
    | readStmt { $$ = $1; }
    | stopStmt { $$ = $1; }
    | pauseStmt { $$ = $1; }
    | dimStmt { $$ = $1; }
    | continueStmt { $$ = $1; }
    | fnAssignStmt { $$ = $1; }
    | returnStmt { $$ = $1; }
    | subCallStmt { $$ = $1; }
    | equivStmt { $$ = $1; }
    | commonStmt { $$ = $1; }
    ;

assignStmt:
    loc ASSIGN exp { $$ = opr(ASSIGN,2,$1,$3); }
    ;

ifStmt:
    IF LPAREN exp RPAREN INTLIT COMMA INTLIT COMMA INTLIT { 
            $$ = opr(IF,4,$3,tagCon($5),tagCon($7),tagCon($9)); }
    ;

gotoStmt:
    GOTO INTLIT { $$ = opr(GOTO, 2, NULL, tagCon($2)); }
    ;

varGotoStmt:
    GOTO LPAREN labelList RPAREN COMMA INTID { 
            $$ = opr(GOTO, 2, $3, INT_ID($6)); }
    ;

doStmt:
    DO INTLIT INTID ASSIGN intVal COMMA intVal { 
            $$ = opr(DO,5, tagCon($2), INT_ID($3), $5, $7, NULL); }
    | DO INTLIT INTID ASSIGN intVal COMMA intVal COMMA intVal {
            $$ = opr(DO,5, tagCon($2), INT_ID($3), $5, $7, $9); }
    ;

fmtStmt:
    FORMAT LPAREN fmtList RPAREN { $$ = opr(FORMAT, 1, $3); }
    ;

printStmt:
    PRINT INTLIT COMMA locList { $$ = opr(PRINT, 2, tagCon($2), $4); }
    ;

readStmt:
    READ INTLIT COMMA locList { $$ = opr(READ, 2, tagCon($2), $4); }
    ;

stopStmt:
    STOP { $$ = opr(STOP, 1, NULL); }
    | STOP INTLIT { $$ = opr(STOP, 1, intCon($2)); }
    ;

pauseStmt:
    PAUSE { $$ = opr(PAUSE, 1, NULL); }
    | PAUSE INTLIT { $$ = opr(PAUSE, 1, intCon($2)); }
    ;

dimStmt:
    DIMENSION dimList { $$ = opr(DIMENSION, 1, $2); }
    ;

continueStmt:
    CONTINUE { $$ = opr(CONTINUE, 1, NULL); }
    ;

returnStmt:
    RETURN { $$ = opr(RETURN, 1, NULL); }
    ;

fnAssignStmt:
    fnId LPAREN formalList RPAREN ASSIGN exp { 
            $$ = opr(FNASSIGN,3,$1,$3,$6); }
    ;

subCallStmt:
    CALL id LPAREN actualList RPAREN { $$ = opr(CALL,2,$2,$4); }
    ;


equivStmt:
    EQUIVALENCE LPAREN formalList RPAREN { $$ = opr(EQUIVALENCE,1,$3); }
    ;

commonStmt:
    COMMON LPAREN formalList RPAREN { $$ = opr(COMMON,1,$3); }
    ;


exp:
    exp PLUS exp { $$ = opr(PLUS,2,$1,$3); }
    | exp TIMES exp { $$ = opr(TIMES,2,$1,$3); }
    | exp MINUS exp { $$ = opr(MINUS,2,$1,$3); }
    | exp DIVIDE exp { $$ = opr(DIVIDE,2,$1,$3); }
    | exp POW exp { $$ = opr(POW,2,$1,$3); }
    | MINUS term %prec UMINUS { $$ = opr(UMINUS,1,$2); }
    | term { $$ = $1; }
    ;

term:
    loc { $$ = $1; }
    | INTLIT { $$ = intCon($1); }
    | FLOATLIT { $$ = floatCon($1); }
    | fnCallExp { $$ = $1; }
    | LPAREN exp RPAREN { $$ = $2; }
    ;

fnCallExp:
    fnId LPAREN actualList RPAREN { $$ = opr(FNCALL,2,$1,$3); }
    ;

intVal:
    INTID { $$ = INT_ID($1); }
    | INTLIT { $$ = intCon($1); }
    ;

loc:
    id { $$ = $1; }
    | indexed { $$ = $1; }
    ;

id:
    INTID { $$ = INT_ID($1); }
    | FLOATID { $$ = FLOAT_ID($1); }
    ;

fnId:
    INTFNID { $$ = INT_FN_ID($1); }
    | FLOATFNID { $$ = FLOAT_FN_ID($1); }
    ;

fmt:
    INTFMTLIT { $$ = intFmtId($1); }
    | FLOATFMTLIT { $$ = floatFmtId($1); }
    | EXPFMTLIT { $$ = expFmtId($1); }
    | HOLFMTLIT { $$ = holFmtId($1); }

indexed: /* Used for both function program calls and indexed access :( */
    id LPAREN expList RPAREN { $$ = opr(INDEXED, 2, $1, $3); }
    ;

formalList:
    id COMMA formalList { $$ = opr(FORMALLIST, 2, $1, $3); }
    | id { $$ = $1; }
    ;


actualList:
    exp COMMA actualList { $$ = opr(ACTUALLIST, 2, $1, $3); }
    | exp { $$ = $1; }
    ;

labelList:
    INTLIT COMMA labelList { $$ = opr(LABELLIST, 2, tagCon($1), $3); }
    | INTLIT { $$ = tagCon($1); }
    ;

fmtList:
    fmt COMMA fmtList { $$ = opr(FMTLIST, 2, $1, $3); }
    | fmt { $$ = $1; }
    ;

locList:
    loc COMMA locList { $$ = opr(LOCLIST, 2, $1, $3); }
    | loc { $$ = $1; }
    ;

dimList:
    indexed COMMA dimList{ $$ = opr(DIMLIST, 2, $1, $3); }
    | indexed { $$ = $1; }
    ;

expList:
    exp COMMA expList { $$ = opr(EXPLIST, 2, $1, $3); }
    | exp { $$ = $1; }
    ;

%% 
#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p) 

nodeType *intCon(struct LeafNode value) { 
    nodeType *p; 
    /* allocate node */ 
    if ((p = malloc(sizeof(nodeType))) == NULL) 
        yyerror("out of memory"); 
    /* copy information */ 
    setLocation();
    p->opr.oper = INTLIT; 
    p->iVal.value = value.iVal; 
    return p; 
} 

nodeType *tagCon(struct LeafNode value) { 
    nodeType *p; 
    /* allocate node */ 
    if ((p = malloc(sizeof(nodeType))) == NULL) 
        yyerror("out of memory"); 
    /* copy information */ 
    setLocation();
    p->opr.oper = TAG; 
    p->iVal.value = value.iVal; 
    return p; 
} 

nodeType *floatCon(struct LeafNode value) { 
    nodeType *p; 
    /* allocate node */ 
    if ((p = malloc(sizeof(nodeType))) == NULL) 
        yyerror("out of memory"); 
    /* copy information */ 
    setLocation();
    p->opr.oper = FLOATLIT; 
    p->fVal.value = value.fVal; 
    return p; 
} 

nodeType *id(struct LeafNode value,int type) { 
    nodeType *p; 
    /* allocate node */ 
    if ((p = malloc(sizeof(nodeType))) == NULL) 
        yyerror("out of memory"); 
    /* copy information */ 
    setLocation();
    p->opr.oper = type; 
    p->sVal.s = value.sPtr; 
    return p; 
} 

nodeType *opr(int oper, int nops, ...) {
    va_list ap; 
    nodeType *p; 
    int i; 
    /* allocate node, extending op array */ 
    if ((p = malloc(sizeof(nodeType))) == NULL) 
        yyerror("out of memory"); 
    if ((p -> opr.op = malloc(nops*sizeof(nodeType *))) == NULL) 
        yyerror("out of memory"); 
    /* copy information */ 
    p->lineno = -1;
    p->charno = -1;
    p->opr.oper = oper; 
    p->opr.nops = nops; 
    va_start(ap, nops); 
    for (i = 0; i < nops; i++) 
        p ->opr.op[i] = va_arg(ap, nodeType*); 
    va_end(ap); 
    return p; 
} 
void freeNode(nodeType *p) { 
}

void printErrorLine(int lineno, int charno){
    char buffer[1024];
    fseek(IN_FILE,0,SEEK_SET);
    for(int i=0;i<lineno;i++)
        fgets(buffer,1024,IN_FILE);
    printf("%s",buffer);
    for(int i=1; i<charno;i++)
        printf(" ");
    printf("^~~~\n");
}

void yyerror(char *s) { 
    fprintf(stdout, "%d,%d: %s\n", yylineno, yycharno, s); 
    printErrorLine(yylineno,yycharno);
    exit(1);
} 

