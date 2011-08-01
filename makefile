CC=g++
HDR = headers/
SRC = source/
CFLAGS=-fPIC -c -I "./headers"
OBJ = Node.o NeuralElement.o Neuron_base.o Connection_base.o SignalOperand.o State.o NodeGroup.o

libBenoit.so : $(OBJ)
	ld -shared -soname libBenoit.so -o libBenoit.so -lc $(OBJ)

Node.o : $(SRC)Node.cpp $(HDR)Node.h $(HDR)NeuralElement.h $(HDR)Connection_base.h $(HDR)Benoit_base.h
	$(CC) $(CFLAGS) $(SRC)Node.cpp

NeuralElement.o : $(SRC)NeuralElement.cpp $(HDR)NeuralElement.h $(HDR)SignalOperand.h $(HDR)State.h $(HDR)Benoit_base.h
	$(CC) $(CFLAGS) $(SRC)NeuralElement.cpp

Connection_base.o : $(HDR)Node.h $(HDR)NeuralElement.h $(SRC)Connection_base.cpp $(HDR)Connection_base.h
	$(CC) $(CFLAGS) $(SRC)Connection_base.cpp

Neuron_base.o : $(HDR)Node.h $(HDR)NeuralElement.h $(HDR)Connection_base.h $(SRC)Neuron_base.cpp $(HDR)Neuron_base.h 
	$(CC) $(CFLAGS) $(SRC)Neuron_base.cpp
	
SignalOperand.o : $(SRC)SignalOperand.cpp $(HDR)SignalOperand.h $(HDR)State.h
	$(CC) $(CFLAGS) $(SRC)SignalOperand.cpp 
	
State.o : $(SRC)State.cpp $(HDR)State.h
	$(CC) $(CFLAGS) $(SRC)State.cpp 
	
NodeGroup.o : $(SRC)NodeGroup.cpp $(HDR)NodeGroup.h $(HDR)Node.h
	$(CC) $(CFLAGS) $(SRC)NodeGroup.cpp
	
ActivationFcn.o : $(SRC)ActivationFcn.cpp $(HDR)ActivationFcn.h
	$(CC) $(CFLAGS) $(SRC)ActivationFcn.cpp 
	
Neuron.o : $(HDR)Node.h $(HDR)NeuralElement.h $(HDR)Connection_base.h $(HDR)Neuron_base.h $(SRC)Neuron.cpp $(HDR)Neuron.h $(HDR)ActivationFcn.h
	$(CC) $(CFLAGS) $(SRC)Neuron.cpp
	
Benoit_base.o : $(HDR)Benoit_base.h $(SRC)Benoit_base.cpp
	$(CC) $(CFLAGS) $(SRC)Benoit_base.cpp
	
