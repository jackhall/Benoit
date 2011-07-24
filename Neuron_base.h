#ifndef Neuron_base_h
#define Neuron_base_h

#include <iostream>
#include <deque>
#include <assert.h>
#include "NeuralElement.h"
#include "Node.h"
#include "Connection_base.h"

class Neuron_base : public NeuralElement, public Node {
private:
	Neuron_base() 
	: NeuralElement(NULL,true,1), Node(0.0,false) {} //hidden default constructor
	void deleteConnections(deque<Connection_base*>& qConn); 
	virtual Connection_base& connectInput(Connection_base* const pNew);
	virtual Connection_base& connectOutput(Connection_base* const pNew);
	virtual void disconnectInput(const Connection_base* const pOld);
	virtual void disconnectOutput(const Connection_base* const pOld);

protected:
	deque<Connection_base*> mqConnIn; //pointers input connections
	deque<Connection_base*> mqConnOut; //pointers output connections
	
	virtual ostream& printNode(ostream &out) const;
	virtual ostream& printElement(ostream& out) const;
	
public:
	Neuron_base(const int nID,
				const SignalOperand* pSignalOperand,
				const unsigned int nTimeSteps=1,
				const double dWeight=1.0,
				const bool bTrainable=true);
	virtual ~Neuron_base();
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;
	virtual Connection_base& addInput(	const Node* pNewIn,
					 		  			const SignalOperand* pSignalOperand,
					 		  			const unsigned int nTimeSteps) = 0;
	virtual Connection_base& addOutput( const Node* pNewOut,
								  		const SignalOperand* pSignalOperand,
						  		  		const unsigned int nTimeSteps) = 0;
	Neuron_base& removeInput(Connection_base* pOldIn);
	Neuron_base& removeOutput(Connection_base* pOldOut);
};

#endif
