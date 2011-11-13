CC	=	g++
HDR	=	headers/
SRC	=	source/
CFLAGS	=	-c -I "./source"
OBJ	=	Node.o Index.o Link.o

libBenoit.so : $(OBJ)
	ld -shared -soname libBenoit.so -o libBenoit.so -lc $(OBJ)

Node.o : $(SRC)Node.cpp $(HDR)Node.h $(HDR)Link.h $(HDR)Index.h
	$(CC) $(CFLAGS) $(SRC)Node.h

Index.o : $(SRC)Index.cpp $(HDR)Node.h $(HDR)Link.h $(HDR)Index.h
	$(CC) $(CFLAGS) $(SRC)Index.h

Link.o : $(SRC)Link.cpp $(HDR)Node.h $(HDR)Link.h $(HDR)Index.h
	$(CC) $(CFLAGS) $(SRC)Link.h


clean : 
	rm *~
	rm source/*~
	rm headers/*~
	rm $(OBJ) libBenoit.so



install :

