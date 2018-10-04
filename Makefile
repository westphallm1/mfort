fortran63.yy.c: fortran63.lex fortran63.tab.h fortran63.h
	lex -o fortran63.yy.c fortran63.lex

fortran63.tab.c: fortran63.y fortran63.h
	bison -d fortran63.y

fortran63.tab.h: fortran63.y fortran63.h
	bison -d fortran63.y

fortran63.yy.o: fortran63.yy.c fortran63.h
	gcc $(CFLAGS) -c -o fortran63.yy.o fortran63.yy.c

fortran63.tab.o: fortran63.tab.c fortran63.tab.h fortran63.h
	gcc $(CFLAGS) -c -Wno-discarded-qualifiers -o fortran63.tab.o fortran63.tab.c

symtable.o: symtable.c symtable.h fortran63.h
	gcc $(CFLAGS) -c -o symtable.o symtable.c

unparser: unparser.c fortran63.yy.o fortran63.tab.o fortran63.h
	gcc $(CFLAGS) -o unparser unparser.c fortran63.yy.o fortran63.tab.o

nameanalysis: run_na.c nameanalysis.o fortran63.yy.o fortran63.tab.o \
	symtable.o fortran63.h nameanalysis.h
	gcc $(CFLAGS) -o nameanalysis run_na.c nameanalysis.o fortran63.yy.o \
		fortran63.tab.o symtable.o

nameanalysis.o: nameanalysis.c nameanalysis.h symtable.h fortran63.yy.c
	gcc $(CFLAGS) -c -o nameanalysis.o nameanalysis.c 

typeanalysis: run_ta.c typeanalysis.o nameanalysis.o fortran63.yy.o\
   	fortran63.tab.o symtable.o fortran63.h nameanalysis.h
	gcc $(CFLAGS) -o typeanalysis run_ta.c typeanalysis.o nameanalysis.o \
		fortran63.yy.o fortran63.tab.o symtable.o

typeanalysis.o: typeanalysis.c typeanalysis.h symtable.h fortran63.yy.c
	gcc $(CFLAGS) -c -o typeanalysis.o typeanalysis.c

scanner: scanner.c fortran63.yy.o fortran63.tab.o fortran63.h
	gcc $(CFLAGS) -o scanner scanner.c fortran63.yy.o fortran63.tab.o

clean:
	rm -f nameanalysis unparser scanner *.o *.tab.h *.yy.h
