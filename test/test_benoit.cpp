#include "Benoit.h"
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the source directory:
//	g++ -std=c++0x -I ~/Code/Benoit/src test_benoit.cpp -o test
//	./test

int main() {
	using namespace ben;
	
	Index<double,double> first_index;
	
	Node<double,double>* pfirst_node = new Node<double,double>(&first_index);
	Node<double,double>* psecond_node = new Node<double,double>(&first_index);
	Node<double,double>* pthird_node = new Node<double,double>(&first_index);
	/*
		instantiations call:
		Node::Node() 
		Node::Node(Index* const) 
		Node::operator=(const Node&) 
	*/
	
	/*
	///////////////////////////
	///////test Nodes and Links
	try ;
	catch {
	
	} //create links
	//calls:
	//Node::add_input(const unsigned int, const W&)	[ 
	//Node::add_output(Link* const) 	[
	
	//-------------------
	try ;
	catch {
	
	} //copy Node w/ input Links
	//call:
	//Node::Node(const Node&)
	
	//-----------------
	try ;
	catch {
	
	} //transmit data
	
	//-----------------
	try ;
	catch {
	
	} //destroy links
	
	//////////////////////////
	///////test Index
	try ;
	catch {
	
	} //
	
	*/
	
	delete pfirst_node, psecond_node, pthird_node;
	return 0;
}
