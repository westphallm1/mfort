%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "fortran63.h"
#include "fortran63.tab.h"
void yyerror(char *);
char yycharno = 1;

#define SAVE_POS() yylval.leafNode.lineno = yylineno; \
                  yylval.leafNode.charno = yycharno;
#define MOVE_POS() yycharno+=yyleng;
#define NEW_LINE() yycharno = 1; yylineno += 1;
#define YY_SPTR() yylval.leafNode.sPtr=malloc(yyleng+1);\
                 strcpy(yylval.leafNode.sPtr,yytext);


int atotag(char * a){
    char * s , *e;
    for(s=a;isspace(*s);s++);
    if(*s == '\0') return -1; // all spaces = no tag 
    for(e=s;isdigit(*e);e++);
    *e='\0';
    return atoi(s);
}

FILE * IN_FILE;
void setInput(FILE * in){
    yyin = in;
    IN_FILE = fdopen (dup (fileno (in)), "r");
}

%}

INTSTART   [I-N]
FLOATSTART [A-HO-Z]
INTFNSTART  X
FLOATFNSTART  [A-WYZ]
IDCHAR [A-Z0-9]
NOTF [A-EG-Z0-9]

%%
"=" { MOVE_POS(); return ASSIGN; }
"CALL" { MOVE_POS(); return CALL; }
"," { MOVE_POS(); return COMMA; }
"COMMON" { MOVE_POS(); return COMMON; }
"CONTINUE" { MOVE_POS(); return CONTINUE; }
"DIMENSION" { MOVE_POS(); return DIMENSION; }
"DO" { MOVE_POS(); return DO; }
"END" { MOVE_POS(); return END; }
"EQUIVALENCE" { MOVE_POS(); return EQUIVALENCE; }
"FORMAT" { MOVE_POS(); return FORMAT; }
"FUNCTION" { MOVE_POS(); return FUNCTION; }
"GO TO" { MOVE_POS(); return GOTO; }
"IF" { MOVE_POS(); return IF; }
"(" { MOVE_POS(); return LPAREN; }
\n { NEW_LINE(); return NEWLINE; }
"PAUSE" { MOVE_POS(); return PAUSE; }
"PRINT" { MOVE_POS(); return PRINT; }
"READ" { MOVE_POS(); return READ; }
"RETURN" { MOVE_POS(); return RETURN; }
")" { MOVE_POS(); return RPAREN; }
"STOP" { MOVE_POS(); return STOP; }
"SUBPROCESS" { MOVE_POS(); return SUBPROCESS; }
"+" { MOVE_POS(); return PLUS; }
"-" { MOVE_POS(); return MINUS; }
"*" { MOVE_POS(); return TIMES; }
"/" { MOVE_POS(); return DIVIDE; }
"**" { MOVE_POS(); return POW; }

{INTSTART}({IDCHAR}{0,2}|{IDCHAR}{2,}{NOTF}) {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return INTID;
        }

{FLOATSTART}({IDCHAR}{0,2}|{IDCHAR}{2,}{NOTF}) {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return FLOATID;
        }

{INTFNSTART}{IDCHAR}{2,}"F" {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return INTFNID;
        }

{FLOATFNSTART}{IDCHAR}{2,}"F" {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return INTFNID;
        }

[0-9]+  {
            SAVE_POS();
            MOVE_POS();
            yylval.leafNode.iVal= atoi(yytext);
            return INTLIT;
        }

[0-9]+"."[0-9]*  {
            SAVE_POS();
            MOVE_POS();
            yylval.leafNode.fVal = atof(yytext);
            return FLOATLIT;
        }

"."[0-9]+  {
            SAVE_POS();
            MOVE_POS();
            yylval.leafNode.fVal = atof(yytext);
            return FLOATLIT;
        }

[0-9]+"."?[0-9]*"E"[+-]?[0-9]{1,2}  {
            SAVE_POS();
            MOVE_POS();
            yylval.leafNode.fVal = atof(yytext);
            return FLOATLIT;
        }

^[ 0-9]{5}[ 0] {
            SAVE_POS();
            MOVE_POS();
            yylval.leafNode.iVal = atotag(yytext);
            return TAG;
        }

[0-9]*"I"[0-9]+ {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return INTFMTLIT;
        }

[0-9]*"H" {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return HOLFMTLIT;
        }

[0-9]*"F"[0-9]+"."[0-9]+ {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return FLOATFMTLIT;
        }

[0-9]*"E"[0-9]+"."[0-9]+ {
            SAVE_POS();
            MOVE_POS();
            YY_SPTR();
            return EXPFMTLIT;
        }

\n"     "[^0 ] {yylineno+=1;yycharno=yyleng-1;}
^[cC][^\n]*\n    {NEW_LINE()}

" "  {MOVE_POS();}
.    {MOVE_POS(); yyerror("Unrecognized character");}


%%
int yywrap(void) {
    return 1;
}
