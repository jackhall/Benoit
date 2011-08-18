#ifndef Node_h
#define Node_h

/*
	Benoit::Node 2011 - copyright Jack Hall
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

class Node : public Benoit_base {
/*
	- A Node inherits from Benoit_base to help standardize the library's debugging interface. 
	- Each Node is identified by a unique unsigned int. These ints are associated with pointers
		to each Node via a static STL map. A read-only access function thus allows a programmer to 
		address Nodes by number as well as by pointer (run-independence). 
	- There are method interfaces for adding and removing connections by Node pointer or number. 
		This is necessary because constructors and destructors must be called for the connections. 
	- fire and backPropagate execute the Node running forwards and backwards, respectively.
	- Overloaded increment and decrement operators let you flip through different time steps.
	- Copy constructing is allowed, but a new ID is assigned to the copy. Overwriting an existing Node 
		via the assignment operator is meaningless for a neural network, and is disallowed. 
	- A boolean member gives a quick way to tell between single types and group types.
*/

private:
	static unsigned int snID_COUNT;	
	static map<unsigned int, Node*> smID_MAP; //one centralized copy avoids sychronization issues

	static unsigned int getNewID() { return snID_COUNT++; }
	Node(); 									//hidden default constructor
	Node& operator=(const Node& cSource);		//hidden assignment operator

protected:
	Node(const bool bGroup) 
		: ID(getNewID()), mbGroup(bGroup) {}
	Node(const Node& cSource)
		: ID(getNewID()), mbGroup(cSource.mbGroup) {}
	virtual ostream& print(ostream &out) const; //not finished

public: 
	const unsigned int ID;
	const bool mbGroup;
	
	static inline Node* find(const unsigned int nAddress) //access a Node via its integer ID
		{ return smID_MAP[nAddress]; }
	virtual ~Node() {}
	virtual Node& operator++() = 0; //advance one time step, 		//defined in Neuron, Ganglion, NeuralNet
	virtual Node& operator--() = 0; //go back one time step, 		//defined in Neuron, Ganglion, NeuralNet
	virtual Node& fire() = 0;											//defined in Neuron, Ganglion, NeuralNet
	virtual Node& backPropagate(const unsigned int nStepsBack) = 0;		//defined in Neuron, Ganglion, NeuralNet
	virtual Node& addInput(	const Node* pNewIn,
							const char chOperator='*',
							const unsigned int nTimeDelay=0) = 0; 	//defined in Neuron, Ganglion, NeuralNet
	Node& addInput(	const unsigned int nNewIn,
					const char chOperator='*',
					const unsigned int nTimeDelay=0);
	virtual Node& addOutput(const Node* pNewOut,
							const char chOperator='*',
							const unsigned int nTimeDelay=0) = 0; 	//defined in Neuron, Ganglion, NeuralNet
	Node& addOutput(const unsigned int nNewOut,
					const char chOperator='*',
					const unsigned int nTimeDelay=0);
	virtual Node& removeInput(Node* pOldIn) = 0;	//defined in Neuron_base, NodeGroup
	Node& removeInput(const unsigned int nOldIn);	//delegates to previous
	virtual Node& removeOutput(Node* pOldOut) = 0;	//defined in Neuron_base, NodeGroup
	Node& removeOutput(const unsigned int nOldOut);	//delegates to previous
};

#endif
