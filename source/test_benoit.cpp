#include "Benoit.h"

//g++ -std=c++0x -I ~/Code/Benoit/headers -I ~/Code/Benoit/source test_benoit.cpp -o test

int main() {
	using namespace ben;
	
	Index<double,double> first_index;
	
	Node<double,double>* pfirst_node = new Node<double,double>(&first_index);
	Node<double,double>* psecond_node = new Node<double,double>(&first_index);
	
	delete pfirst_node, psecond_node;
	return 0;
}
