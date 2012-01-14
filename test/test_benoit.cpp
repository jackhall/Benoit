#include "Benoit.h"
#include <iostream>
#include <list>
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the test directory:
//	g++ -std=c++0x -g -I ../src test_benoit.cpp -o test
//	./test

int test_one(); //test Node & Index constructors
int test_two(); //test Link constructors & management

int main() {
	using namespace std;
	
	list<int> results;
	results.push_back( test_one() );
	results.push_back( test_two() );
	
	//test data transmission
	
	//test Node management (Index methods)
	
	//test destruction: Links, Nodes/Index
	
	int test_number = 1;
	auto it = results.begin();
	auto ite = results.end();
	while(it != ite) {
		cout << "Test " << test_number;
		if(*it == 0) {
			cout << " passed." << endl;
		} else {
			cout << " failed with error code " << *it << "." << endl;
		}
		++it;
		++test_number;
	}
	return 0;
} //main

int test_one() { //test Node & Index constructors
	
	using namespace ben;
	
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
	Node<double,double>* pthird_node = new Node<double,double>(std::move(*psecond_node));
	delete psecond_node; //psecond_node shares an ID with pthird_node, so delete it
	psecond_node = NULL; 
	//testing Node::operator= (creates new copies of input links)
	*pthird_node = *pfirst_node; 
	
	//testing Index copy constructor
	Index<double,double>* psecond_index = new Index<double,double>(std::move(first_index)); 
	
	//testing destructors
	delete pfirst_node;
	delete pthird_node;
	delete psecond_index;
	
	return 0;
} //test_one

int test_two() { //test Link constructors & management
	using namespace ben;
	
	Node<double,double> node_one;
	node_one.bias = 3.0;
	Node<double,double>* pnode_two = new Node<double,double>();
	pnode_two->bias = 5.0;
	
	//test basic add/remove
	node_one.add_input(pnode_two->ID,7.0);
	node_one.contains_input(pnode_two->ID);
	pnode_two->contains_output(node_one.ID);
	
	node_one.remove_input(pnode_two->ID); 
	node_one.contains_input(pnode_two->ID);
	pnode_two->contains_output(node_one.ID);
	
	//test self-Linking
	pnode_two->add_input(pnode_two->ID,11.0);
	pnode_two->contains_input(pnode_two->ID);
	pnode_two->contains_output(pnode_two->ID);
	
	//test basic clear on self-Link
	pnode_two->clear();
	
	//test clearing from output end
	node_one.add_input(pnode_two->ID,13.0);
	pnode_two->clear();
	node_one.contains_input(pnode_two->ID);
	
	//test clearing from input end
	node_one.add_input(pnode_two->ID,17.0);
	node_one.clear(); 
	pnode_two->contains_output(node_one.ID);
	
	delete pnode_two;
	
	return 0;
} //test_two

