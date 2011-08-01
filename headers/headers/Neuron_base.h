#ifndef Neuron_base_h
#define Neuron_base_h

#include <iostream>
#include <deque>
#include "NeuralElement.h"
#include "Node.h"
#include "Connection_base.h"

class Neuron_base : public NeuralElement, public Node {
private:
	Neuron_base(); //hidden default constructor
	void deleteConnections(deque<Connection_base*>& qConn); 
	Connection_base& connectInput(Connection_base* const pNew);
	Connection_base& connectOutput(Connection_base* const pNew);
	void disconnectInput(const Connection_base* const pOld);
	void disconnectOutput(const Connection_base* const pOld);
	Neuron_base& operator=(const Neuron_base& cSource);

protected:
	deque<Connection_base*> mqConnIn; //pointers input connections
	deque<Connection_base*> mqConnOut; //pointers output connections
	State mcOutputBuffer;
	
	State& collectSignals();
	State& collectErrors();
	void distributeOutput() const;
	void distributeError() const;
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	Neuron_base(const int nID,
				const SignalOperand* pSignalOperand,
				const unsigned int nTimeSteps=1,
				const double dWeight=1.0,
				const bool bTrainable=true);
	Neuron_base(const Neuron_base& cSource);
	virtual ~Neuron_base();
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;
	Neuron_base& removeInput(Connection_base* pOldIn);
	Neuron_base& removeOutput(Connection_base* pOldOut);
};

#endif
