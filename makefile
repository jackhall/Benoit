CC	=	g++
HDR	=	headers/
SRC	=	source/
CFLAGS=	-fPIC -c -I "./headers"
CFLAGS_TMPL	= -c -I "./source"
OBJ	=	Benoit_base.o Node.o Neuron_base.o Connection_base.o NodeGroup.o
TMPL=	State.o Connection.o Neuron.o

libBenoit.so : $(OBJ)
	ld -shared -soname libBenoit.so -o libBenoit.so -lc $(OBJ)

Node.o : $(SRC)Node.cpp $(HDR)Node.h $(HDR)Connection_base.h $(HDR)Benoit_base.h
	$(CC) $(CFLAGS) $(SRC)Node.cpp

Connection_base.o : $(HDR)Node.h $(SRC)Connection_base.cpp $(HDR)Connection_base.h $(HDR)Benoit_base.h $(HDR)Neuron_base.h
	$(CC) $(CFLAGS) $(SRC)Connection_base.cpp

Neuron_base.o : $(HDR)Node.h $(HDR)Connection_base.h $(SRC)Neuron_base.cpp $(HDR)Neuron_base.h
	$(CC) $(CFLAGS) $(SRC)Neuron_base.cpp
	
NodeGroup.o : $(SRC)NodeGroup.cpp $(HDR)NodeGroup.h $(HDR)Node.h
	$(CC) $(CFLAGS) $(SRC)NodeGroup.cpp
	
Benoit_base.o : $(HDR)Benoit_base.h $(SRC)Benoit_base.cpp
	$(CC) $(CFLAGS) $(SRC)Benoit_base.cpp




templates : $(TMPL)

State.o : $(HDR)State.h $(SRC)State.cpp
	$(CC) $(CFLAGS_TMPL) $(HDR)State.h
	
Connection.o : $(HDR)Connection.h $(SRC)Connection.cpp
	$(CC) $(CFLAGS_TMPL) $(HDR)Connection.h
	
Neuron.o : $(HDR)Neuron.h $(SRC)Neuron.cpp
	$(CC) $(CFLAGS_TMPL) $(HDR)Neuron.h



clean : 
	rm *~
	rm source/*~
	rm headers/*~
	rm $(OBJ) libBenoit.so



install :

