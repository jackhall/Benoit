#ifndef Node_h
#define Node_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	A Node is the basic independent causal unit in a neural network. 
	The concept is similar to a directed graph in mathematics. The execution 
	of a Node does not depend on any single other part of the network 
	(special cases aside), but on many. A Node can be a Neuron, a group of 
	neurons independent of each other (Ganglion), or an entire network (NeuralNet). 
	This allows for a recursive hierarchy of Nodes. 
*/

#include <iostream>
#include <map>
#include "Benoit_base.h"

using namespace std;

template<typename T, typename U>
class Node {
/*
	- Each Node is identified by a unique unsigned int. These ints are associated with pointers
		to each Node via a static STL map. A read-only access function thus allows a programmer to 
		address Nodes by number as well as by pointer (run-independence). 
	- There are method interfaces for adding and removing connections by Node pointer or number. 
		This is necessary because constructors and destructors must be called for the connections. 
*/

private:
	static unsigned int snID_COUNT;	
	static map<unsigned int, Node*> smID_MAP; 	//one centralized copy avoids sychronization issues

	static unsigned int getNewID() { return snID_COUNT++; }
	Node& operator=(const Node& cSource);		//hidden assignment operator
	Node(const Node& cSource); 					//hidden copy constructor

protected:
	virtual ostream& print(ostream &out) const; //not finished

public: 
	//Node ID and indexing
	const unsigned int ID;
	static inline Node* find(const unsigned int nAddress) //access a Node via its integer ID
		{ return smID_MAP[nAddress]; }
	
	//constructor, destructor
	Node();
	virtual ~Node() {}
	
	//connection management
	Node& addInput(	const Node* pNewIn,
					const bool bTimeDelay=false); 
	Node& addInput(	const unsigned int nNewIn,		//delegates to previous
					const bool bTimeDelay=false);
	Node& addOutput(const Node* pNewOut,
					const bool bTimeDelay=false); 
	Node& addOutput(const unsigned int nNewOut,		//delegates to previous
					const bool bTimeDelay=false);
	Node& removeInput(Node* pOldIn);
	Node& removeInput(const unsigned int nOldIn);	//delegates to previous
	Node& removeOutput(Node* pOldOut);
	Node& removeOutput(const unsigned int nOldOut);	//delegates to previous
	
	//iterators
	class input_iter {
	private:
	
	public:
	
	};
	
	class output_iter {
	private:
	
	public:
	
	};
	
	input_iter inputBegin();
	input_iter inputEnd();
	output_iter outputBegin();
	output_iter outputEnd();
};

#include "Node.cpp"

#endif
