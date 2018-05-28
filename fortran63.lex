%{
#include <stdlib.h>
#include "fortran63.h"
#include "fortran63.tab.h"
void yyerror(char *);
char yycharno = 1;

#define movepos() yycharno+=yyleng;
#define newline() yycharno = 1; yylineno += 1;
#define yysPtr() yylval.sPtr = malloc(yyleng+1);strcpy(yylval.sPtr,yytext);


int atotag(char * a){
    return -1;
}

%}

INTSTART   [IJKLMN]
FLOATSTART [A-HO-Z]
INTFNSTART  X
FLOATFNSTART  [A-WYZ]
IDCHAR [A-Z0-9]
NOTF [A-EG-Z0-9]

%%
"=" { movepos(); return ASSIGN; }
"CALL" { movepos(); return CALL; }
"," { movepos(); return COMMA; }
"COMMON" { movepos(); return COMMON; }
"CONTINUE" { movepos(); return CONTINUE; }
"DIMENSION" { movepos(); return DIMENSION; }
"DO" { movepos(); return DO; }
"END" { movepos(); return END; }
"EQUIVALENCE" { movepos(); return EQUIVALENCE; }
"FORMAT" { movepos(); return FORMAT; }
"FUNCTION" { movepos(); return FUNCTION; }
"GO TO" { movepos(); return GOTO; }
"IF" { movepos(); return IF; }
"(" { movepos(); return LPAREN; }
\n { newline(); return NEWLINE; }
"PAUSE" { movepos(); return PAUSE; }
"PRINT" { movepos(); return PRINT; }
"READ" { movepos(); return READ; }
"RETURN" { movepos(); return RETURN; }
")" { movepos(); return RPAREN; }
"STOP" { movepos(); return STOP; }
"SUBPROCESS" { movepos(); return SUBPROCESS; }
"+" { movepos(); return PLUS; }
"-" { movepos(); return MINUS; }
"*" { movepos(); return TIMES; }
"/" { movepos(); return DIVIDE; }
"**" { movepos(); return POW; }

{INTSTART}({IDCHAR}{0,2}|{IDCHAR}{2,}{NOTF}) {
            movepos();
            yysPtr();
            return INTID;
        }

{FLOATSTART}({IDCHAR}{0,2}|{IDCHAR}{2,}{NOTF}) {
            movepos();
            yysPtr();
            return FLOATID;
        }

{INTFNSTART}{IDCHAR}{2,}"F" {
            movepos();
            yysPtr();
            return INTFNID;
        }

{FLOATFNSTART}{IDCHAR}{2,}"F" {
            movepos();
            yysPtr();
            return INTFNID;
        }

[0-9]+  {
            movepos();
            yylval.iValue= atoi(yytext);
            return INTLIT;
        }

[0-9]+"."[0-9]*  {
            movepos();
            yylval.fValue= atof(yytext);
            return FLOATLIT;
        }

"."[0-9]+  {
            movepos();
            yylval.fValue= atof(yytext);
            return FLOATLIT;
        }

[0-9]+"."?[0-9]*"E"[+-]?[0-9]{1,2}  {
            movepos();
            yylval.fValue= atof(yytext);
            return FLOATLIT;
        }

^[ 0-9]{5}[ 0] {
            movepos();
            yylval.iValue= atotag(yytext);
            return TAG;
        }

[0-9]*"I"[0-9]+ {
            movepos();
            yysPtr();
            return INTFMTLIT;
        }

[0-9]*"H" {
            movepos();
            yysPtr();
            return HOLFMTLIT;
        }

[0-9]*"F"[0-9]+"."[0-9]+ {
            movepos();
            yysPtr();
            return FLOATFMTLIT;
        }

[0-9]*"E"[0-9]+"."[0-9]+ {
            movepos();
            yysPtr();
            return EXPFMTLIT;
        }

\n"     "[^0 ] {yylineno+=1;yycharno=yyleng-1;}
"c"[^\n]*\n    {newline()}

" "  {movepos();}
.    {movepos(); yyerror("Unrecognized character");}


%%
int yywrap(void) {
    return 1;
}
