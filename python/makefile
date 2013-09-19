CC = g++
CFLAGS = -std=c++11 -lpthread -g -march=native
LFLAGS = -shared -Wl,-no-undefined -g
PFLAGS = -lpthread -lpython2.7 -lboost_python -lboost_system
BASE = ..
SRC = $(BASE)/src
PATHS = -I$(SRC)

build/benpy.so : python_utils.h $(SRC)/Index.h $(SRC)/UndirectedNode.h $(SRC)/DirectedNode.h $(SRC)/LinkManager.h $(SRC)/Port.h $(SRC)/Path.h $(SRC)/Buffer.h benoitpy.cpp
	$(CC) $(CFLAGS) $(PATHS) -fPIC -I/usr/include/python2.7 -o benpy.o -c benoitpy.cpp
	$(CC) $(LFLAGS) $(PFLAGS) -o benpy.so benpy.o
	rm build/*.o

#build/libbenoit.so : build/IndexBase.o
#	$(CC) $(LFLAGS) -lboost_system -Wl,-soname,libbenoit.so.1 -o build/libbenoit.so.1.0 build/IndexBase.o; \
	cd build; \
	ln -s libbenoit.so.1.0 libbenoit.so.1; \
	ln -s libbenoit.so.1 libbenoit.so

#build/IndexBase.o : src/IndexBase.cpp src/IndexBase.h src/Singleton.h
#	$(CC) $(CFLAGS) $(PATHS) -fPIC -c src/IndexBase.cpp; \
	mv IndexBase.o build/.

#install : 
#	sudo mv build/libbenoit.so /usr/lib64/.; \
	sudo mv build/libbenoit.so.1.0 /usr/lib64/.; \
	sudo ldconfig
	
#clean :
#	rm build/*.o
	
#remove :
#	rm build/*; \
	sudo rm /usr/lib64/libbenoit.*; \
	sudo ldconfig
	
