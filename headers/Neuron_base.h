#ifndef Neuron_base_h
#define Neuron_base_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	Neuron_base is a single-Node interface class. It provides an interface for the 
	fundamental independent element in a neural network: a Neuron. However, in the 
	interest of keeping Benoit's interface and data structure separate from the 
	implementation of neural computations, Neuron_base stores no data and performs
	no computations. 
	
	This class is tied tightly to the Connection_base class in a way that defines
	the neural data structure. Neuron_bases share ownership of the Connection_bases
	between them, and all objects clean up after themselves when deleted. In the 
	future it may be most efficient to give Neuron_bases individual ownership of
	their Connection_bases, but this would sacrifice speed in at least one direction 
	of propagation and possibly sacrifice flexibility in Connection_base.
*/

#include <iostream>
#include <deque>
#include "Node.h"
#include "Connection_base.h"

class Neuron_base : public Node {
/*
	- Inherits from Node so that it is causally independent and shares a standardized interface.
	- Construction and copying properties are similar to that of Node: can be copy-constructed
		but not copy-overwritten (assignment operator).
	- Pointers to Connection_bases are stored in a pair of deques (double-ended queues). A deque
		is better than a vector because adding new elements cannot cause massive, repeated memory
		allocations. One deque for input connections, one for outputs. 
	- A bool that indicates whether the bias of the Neuron can be adjusted. This interface class 
		has no bias data member, but Neuron (a child class) does. This is public nonconst because
		making it private and providing full public access through methods is wasteful. 
	- There are four public methods for cleanly removing a Connection_base, two of them inherited 
		from Node. The other two are there for convenience.
	- There are four private methods for managing Connection_bases that do not cleanly remove 
		Connection_bases; they merely add or remove pointers from the deques. One more private method
		cleanly removes all inputs or all outputs at once. This last is only called by the destructor.
	- The constructors and destructor of Connection_base are friend methods because they need access
		the private Connection_base management methods in order to manage their pointers and memory. 
	- The fire and backPropagate methods inherited from Node are still not defined in Neuron_base.
*/

private:
	Neuron_base(); 			//hidden default constructor
	Neuron_base& operator=(const Neuron_base& cSource);	//hidden assignment operator
	Connection_base& connectInput(Connection_base* const pNew);	//called only by Connection_base constructor
	Connection_base& connectOutput(Connection_base* const pNew);//called only by Connection_base constructor
	void disconnectInput(const Connection_base* const pOld);	//called only by Connection_base destructor
	void disconnectOutput(const Connection_base* const pOld);	//called only by Connection_base destructor
	void deleteConnections(deque<Connection_base*>& qConn);		//called only by Neuron_base destructor
	friend Connection_base::Connection_base(Neuron_base* pSource,
											Neuron_base* pTarget,
									   		const bool bTrainable);
	friend Connection_base::Connection_base(const unsigned int nSource,		//Node ID version
											const unsigned int nTarget,
											const bool bTrainable);
	friend Connection_base::~Connection_base();

protected:
	deque<Connection_base*> mqConnIn; //pointers input connections
	deque<Connection_base*> mqConnOut; //pointers output connections
	
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	bool mbTrainable;		//full public access allows easy network-level training/diagnostics
	
	Neuron_base(const bool bTrainable=true)
	: Node(false), mbTrainable(bTrainable) {}
	Neuron_base(const Neuron_base& cSource) 
	: Node(cSource), mbTrainable(cSource.mbTrainable) {}
	virtual ~Neuron_base();
	virtual Node& fire() = 0;										//virtual from Node
	virtual Node& backPropagate(const unsigned int nStepsBack) = 0;	//virtual from Node
	Node& removeInput(Node* pOldIn); 			//virtual from Node
	Node& removeOutput(Node* pOldOut); 			//virtual from Node
	Neuron_base& removeInput(Connection_base* pOldIn);		//unique to Neuron_base
	Neuron_base& removeOutput(Connection_base* pOldOut);	//unique to Neuron_base
};

#endif

