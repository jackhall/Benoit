#ifndef Neuron_base_h
#define Neuron_base_h

#include <iostream>
#include <deque>
#include "Node.h"
#include "Connection_base.h"

class Neuron_base : public Node {
private:
	Neuron_base(); //hidden default constructor
	Neuron_base& operator=(const Neuron_base& cSource);
	Connection_base& connectInput(Connection_base* const pNew);
	Connection_base& connectOutput(Connection_base* const pNew);
	void disconnectInput(const Connection_base* const pOld);
	void disconnectOutput(const Connection_base* const pOld);
	void deleteConnections(deque<Connection_base*>& qConn);
	friend Connection_base::Connection_base(Neuron_base* pSource,
											Neuron_base* pTarget,
									   		const bool bTrainable);
	friend Connection_base::Connection_base(const unsigned int nSource,
											const unsigned int nTarget,
											const bool bTrainable);
	friend Connection_base::~Connection_base();

protected:
	deque<Connection_base*> mqConnIn; //pointers input connections
	deque<Connection_base*> mqConnOut; //pointers output connections
	
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	bool mbTrainable;
	
	Neuron_base(const bool bTrainable=true)
	: Node(false), mbTrainable(bTrainable) {}
	Neuron_base(const Neuron_base& cSource) 
	: Node(cSource), mbTrainable(cSource.mbTrainable) {}
	virtual ~Neuron_base();
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;
	Node& removeInput(Node* pOldIn);
	Node& removeOutput(Node* pOldOut);
	Neuron_base& removeInput(Connection_base* pOldIn);
	Neuron_base& removeOutput(Connection_base* pOldOut);
};

#endif

