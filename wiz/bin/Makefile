CFLAGS+=-g -ggdb -O2
#CFLAGS+=wiz/bin/gvc.dll wiz/bin/graph.dll wiz/bin/cdt.dll
#LIBS=-Wl,-rpath-link=wiz/bin/gvc.dll,-rpath-link=wiz/bin/graph.dll,-rpath-link=wiz/bin/cdt.dll
#LIBS+=-rpath wiz\bin\gvc.dll
#LIBS+=-rpath wiz\bin\graph.dll
#LIBS+=-Wl,-R=wiz\bin\gvc.dll,-R=wiz\bin\graph.dll
LIBS+=-L. -lgvc -lgraph

flviz.exe: flviz.o Makefile
#	ld $(LIBS) flviz.o -o flviz
	gcc $(CFLAGS) $(LIBS) flviz.o -o flviz

flviz.o: flviz.c
	gcc $(CFLAGS) -o flviz.o -c flviz.c
