CC = g++
CFLAGS = -std=c++11 -lpthread -lgtest -lbenoit -g 
PATHS = -I../src -I../build -I../Wayne/src
SRC = ../src

test_singleton : $(SRC)/IndexBase.h $(SRC)/IndexBase.cpp $(SRC)/Index.h $(SRC)/Singleton.h test_singleton.cpp 
	$(CC) $(CFLAGS) $(PATHS) test_singleton.cpp -o test_singleton
	
remove :
	rm test_singleton
	
