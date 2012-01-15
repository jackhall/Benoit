#include "Benoit.h"
#include <iostream>
#include <list>
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the test directory:
//	g++ -std=c++0x -g -I ../src test_benoit.cpp -o test
//	./test

int test_one(); //test Node & Index constructors
int test_two(); //test Link constructors & management
int test_three(); //test data transmission
int test_four(); //test Node management (copying, Index methods...)

int main() {
	using namespace std;
	
	list<int> results;
	results.push_back( test_one() );
	results.push_back( test_two() );
	results.push_back( test_three() );
	results.push_back( test_four() );
	
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
	if( !Node<int,int>::INDEX.contains(dummy_node.ID) ) return 1;
	
	//testing Index default constructor
	Index<double,double> first_index;
	
	//testing Node default constructor with new
	Node<double,double>* pfirst_node = new Node<double,double>();
	if( !Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 2;
	
	//testing Index::move_to
	Node<double,double>::INDEX.move_to(first_index, pfirst_node->ID);
	if( Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 3;
	if( !first_index.contains(pfirst_node->ID) ) return 4;
	
	//testing Node(Index*) constructor
	Node<double,double>* psecond_node = new Node<double,double>(&first_index);
	if( !first_index.contains(psecond_node->ID) ) return 5;
	
	//testing Node move constructor
	unsigned int second_ID = psecond_node->ID;
	Node<double,double>* pthird_node = new Node<double,double>(std::move(*psecond_node));
	if( pthird_node->ID != second_ID ) return 6;
	if( first_index.find(psecond_node->ID) == psecond_node ) return 7;
	if( first_index.find(pthird_node->ID) != pthird_node ) return 8;
	delete psecond_node; //psecond_node shares an ID with pthird_node, so delete it
	psecond_node = NULL; 
	
	//testing Node::operator= (creates new copies of input links)
	Node<double,double>* pfourth_node = new Node<double,double>();
	*pthird_node = *pfourth_node; 
	if( first_index.contains(pthird_node->ID) ) return 9;
	if( !Node<double,double>::INDEX.contains(pthird_node->ID) ) return 10;
	
	//testing Index copy constructor
	Index<double,double>* psecond_index = new Index<double,double>(std::move(first_index));
	if( !psecond_index->contains(pfirst_node->ID) ) return 11;
	
	//testing destructors
	delete psecond_index;
	if( !Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 12;
	delete pfirst_node;
	delete pthird_node;
	delete pfourth_node;
	
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
	if( !node_one.contains_input(pnode_two->ID) ) return 1;
	if( !pnode_two->contains_output(node_one.ID) ) return 2;
	
	node_one.remove_input(pnode_two->ID); 
	if( node_one.contains_input(pnode_two->ID) ) return 3;
	if( pnode_two->contains_output(node_one.ID) ) return 4;
	
	//test self-Linking
	pnode_two->add_input(pnode_two->ID,11.0);
	if( !pnode_two->contains_input(pnode_two->ID) ) return 5;
	if( !pnode_two->contains_output(pnode_two->ID) ) return 6;
	
	//test basic clear on self-Link
	pnode_two->clear();
	
	//test clearing from output end
	node_one.add_input(pnode_two->ID,13.0);
	pnode_two->clear();
	if( node_one.contains_input(pnode_two->ID) ) return 7;
	
	//test clearing from input end
	node_one.add_input(pnode_two->ID,17.0);
	node_one.clear(); 
	if( pnode_two->contains_output(node_one.ID) ) return 8;
	
	delete pnode_two;
	
	return 0;
} //test_two

int test_three() { //test data transmission
	using namespace ben;
	
	Node<double,double> node_one, node_two, node_three;
	node_one.bias = 11.0;
	node_two.bias = 13.0;
	node_three.bias = 17.0;
	
	node_three.add_input(node_one.ID,3.0);
	node_three.add_input(node_two.ID,5.0);
	node_two.add_input(node_one.ID,7.0);
	
	//send signal from node_one
	auto po1 = node_one.output_begin();
	auto po1e = node_one.output_end();
	while(po1 != po1e) {
		po1 << 19.0 << 23.0; //23.0 is time-delayed
		++po1;
	}
	
	//receive data at node_two
	auto pi2 = node_two.input_begin();
	auto pi2e = node_two.input_end();
	double x, z=node_two.bias;
	while(pi2 != pi2e) {
		pi2 >> x;
		z *= pi2->weight * x;
		++pi2;
	} 
	if( z != 13.0*(7.0*19.0) ) return 1;
	
	//send product of all numbers to node_three
	auto po2 = node_two.output_begin();
	auto po2e = node_two.output_end();
	while(po2 != po2e) {
		po2 << z;
		++po2;
	}
	
	//receive signals from node_one and node_two at node_three
	auto pi3 = node_three.input_begin();
	auto pi3e = node_three.input_end();
	z = node_three.bias;
	while(pi3 != pi3e) {
		pi3 >> x;
		z *= pi3->weight * x;
		++pi3;
	}
	if( z != 17.0*(3.0*19.0)*(5.0*13.0*(7.0*19.0)) ) return 2;
	
	//transmit from node_one again
	po1 = node_one.output_begin();
	while(po1 != po1e) {
		po1 << 29.0;
		++po1;
	}
	
	//test time delay too
	pi2 = node_two.input_begin();
	z = node_two.bias;
	while(pi2 != pi2e) {
		pi2 >> x;
		z *= pi2->weight * x;
		++pi2;
	}
	if( z != 13.0*(7.0*23.0) ) return 3;
	
	return 0;
}

int test_four() { //test Node management (copying, Index methods...)
	using namespace ben;
	
	return 0;
}
