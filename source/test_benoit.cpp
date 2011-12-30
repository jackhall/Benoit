#include "Benoit.h"
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the source directory:
//	g++ -std=c++0x -I ~/Code/Benoit/headers -I ~/Code/Benoit/source test_benoit.cpp -o test
//	./test

int main() {
	using namespace ben;
	
	Index<double,double> first_index;
	
	Node<double,double>* pfirst_node = new Node<double,double>(&first_index);
	Node<double,double>* psecond_node = new Node<double,double>(&first_index);
	
	delete pfirst_node, psecond_node;
	return 0;
}
