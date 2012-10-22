CC = g++
CFLAGS = -std=c++11 -lpthread -lgtest -lbenoit -g 
PATHS = -I../src -I../build -I../Wayne/src
SRC = ../src

test_singleton : $(SRC)/IndexBase.h $(SRC)/IndexBase.cpp $(SRC)/Index.h $(SRC)/Singleton.h test_singleton.cpp 
	$(CC) $(CFLAGS) $(PATHS) test_singleton.cpp -o test_singleton
	
test_graph : $(SRC)/IndexBase.h $(SRC)/IndexBase.cpp $(SRC)/Index.h $(SRC)/Singleton.h $(SRC)/Graph.h $(SRC)/Node.h $(SRC)/Port.h $(SRC)/Link.h $(SRC)/PushLink.h $(SRC)/PullLink.h test_graph.cpp 
	$(CC) $(CFLAGS) $(PATHS) test_graph.cpp -o test_graph
	
remove :
	rm test_singleton test_graph
	
