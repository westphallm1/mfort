fortran63.yy.c: fortran63.lex fortran63.tab.h fortran63.h
	lex -o fortran63.yy.c fortran63.lex

fortran63.tab.c: fortran63.y fortran63.h
	bison -d fortran63.y

fortran63.tab.h: fortran63.y fortran63.h
	bison -d fortran63.y

fortran63.yy.o: fortran63.yy.c fortran63.h
	gcc -g -c -o fortran63.yy.o fortran63.yy.c

fortran63.tab.o: fortran63.tab.c fortran63.tab.h fortran63.h
	gcc -g -c -Wno-discarded-qualifiers -o fortran63.tab.o fortran63.tab.c

symtable.o: symtable.c symtable.h fortran63.h
	gcc -g -c -o symtable.o symtable.c

unparser: unparser.c fortran63.yy.o fortran63.tab.o fortran63.h
	gcc -g -o unparser unparser.c fortran63.yy.o fortran63.tab.o

nameanalysis: run_na.c nameanalysis.o fortran63.yy.o fortran63.tab.o \
	symtable.o fortran63.h nameanalysis.h
	gcc -g -o nameanalysis run_na.c nameanalysis.o fortran63.yy.o \
		fortran63.tab.o symtable.o

nameanalysis.o: nameanalysis.c nameanalysis.h symtable.h fortran63.yy.c
	gcc -g -c -o nameanalysis.o nameanalysis.c 

typeanalysis: run_ta.c typeanalysis.o nameanalysis.o fortran63.yy.o\
   	fortran63.tab.o symtable.o fortran63.h nameanalysis.h
	gcc -g -o typeanalysis run_ta.c typeanalysis.o nameanalysis.o \
		fortran63.yy.o fortran63.tab.o symtable.o

typeanalysis.o: typeanalysis.c typeanalysis.h symtable.h fortran63.yy.c
	gcc -g -c -o typeanalysis.o typeanalysis.c

scanner: scanner.c fortran63.yy.o fortran63.tab.o fortran63.h
	gcc -g -o scanner scanner.c fortran63.yy.o fortran63.tab.o

clean:
	rm -f nameanalysis unparser scanner *.o *.tab.h *.yy.h
