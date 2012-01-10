#include "Benoit.h"
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the test directory:
//	g++ -std=c++0x -g -I ../src test_benoit.cpp -o test
//	./test

int main() {
	using namespace ben;
	////////////////////////////////////////////
	//////////////test Node & Index constructors
	//testing Node default constructor on stack
	Node<int,int> dummy_node;
	
	//testing Index default constructor
	Index<double,double> first_index;
	
	//testing Node default constructor with new
	Node<double,double>* pfirst_node = new Node<double,double>();
	//testing Index::move_to
	Node<double,double>::INDEX.move_to(first_index, pfirst_node->ID);
	
	//testing Node(Index*) constructor
	Node<double,double>* psecond_node = new Node<double,double>(&first_index);
	//testing Node copy constructor (uses move semantics)
	Node<double,double>* pthird_node = new Node<double,double>(*psecond_node);
	delete psecond_node; //psecond_node shares an ID with pthird_node, so delete it
	psecond_node = NULL;
	//testing Node::operator= (creates new copies of input links)
	*pthird_node = *pfirst_node; 
	
	//testing Index copy constructor
	//segfaults! probably more std::move hijinks
	Index<double,double>* psecond_index = new Index<double,double>(first_index); 
	
	///////////////////////////////////////////
	////////test Link constructors & management
	
	///////////////////////////////////////////
	/////////////////////test data transmission
	
	///////////////////////////////////////////
	///////test Node management (Index methods)
	
	///////////////////////////////////////////
	///////test destruction: Links, Nodes/Index
	
	delete pfirst_node, pthird_node;
	return 0;
}
