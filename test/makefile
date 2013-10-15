CC = g++
CFLAGS = -std=c++11 -lgtest -g -march=native -Wfatal-errors
SRC = ../src
PATHS = -I$(SRC)

test_benoit : $(SRC)/IndexBase.h $(SRC)/IndexBase.cpp $(SRC)/Index.h $(SRC)/Singleton.h $(SRC)/Graph.h $(SRC)/Node.h $(SRC)/Port.h $(SRC)/Path.h $(SRC)/Traits.h test_benoit.cpp 
	$(CC) $(CFLAGS) $(PATHS) test_benoit.cpp -o test_benoit
	
