CC=g++
HDR = headers/
SRC = source/
CFLAGS=-fPIC -c -I "./headers"
OBJ = Node.o NeuralElement.o Neuron_base.o Connection_base.o SignalOperand.o

libBenoit.so : $(OBJ)
	ld -shared -soname libBenoit.so -o libBenoit.so -lc $(OBJ)

Node.o : $(SRC)Node.cpp $(HDR)Node.h $(HDR)NeuralElement.h $(HDR)Connection_base.h
	$(CC) $(CFLAGS) $(SRC)Node.cpp

NeuralElement.o : $(SRC)NeuralElement.cpp $(HDR)NeuralElement.h $(HDR)SignalOperand.h 
	$(CC) $(CFLAGS) $(SRC)NeuralElement.cpp

Connection_base.o : $(HDR)Node.h $(HDR)NeuralElement.h $(SRC)Connection_base.cpp $(HDR)Connection_base.h
	$(CC) $(CFLAGS) $(SRC)Connection_base.cpp

Neuron_base.o : $(HDR)Node.h $(HDR)NeuralElement.h $(HDR)Connection_base.h $(SRC)Neuron_base.cpp $(HDR)Neuron_base.h 
	$(CC) $(CFLAGS) $(SRC)Neuron_base.cpp
	
SignalOperand.o : $(SRC)SignalOperand.cpp $(HDR)SignalOperand.h
	$(CC) $(CFLAGS) $(SRC)SignalOperand.cpp 
	
