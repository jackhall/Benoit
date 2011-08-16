#ifndef Neuron_h
#define Neuron_h

#include <iostream>
#include "Neuron_base.h"
#include "Connection.h"
#include "State.h"

template<typename T>
class Neuron : public Neuron_base {
private:
	Neuron(); //private default constructor
	Neuron& operator=(const Neuron& cSource);
	State<T> msSignal, msOutput, msError, msBuffer;
	
	State<T>& add(Connection_base* pConn);
	State<T>& multiply(Connection_base* pConn);
	State<T>& subtract(Connection_base* pConn);
	
	State<T>& addD(Connection_base* pConn);
	State<T>& multiplyD(Connection_base* pConn);
	State<T>& subtractD(Connection_base* pConn);
	
	State<T>& (*op)(Connection_base*);
	State<T>& (*opderiv)(Connection_base*);
	
	State<T>& tanh();
	State<T>& linear();
	State<T>& rbf();
	
	State<T>& tanhD();
	State<T>& linearD();
	State<T>& rbfD();
	
	State<T>& (*activationfcn)();
	State<T>& (*activationfcnderiv)();

protected:
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	T mtBias;
	Neuron(	const char chActivationFcn,
			const char chOperator="+",
			const unsigned int nSamples=1,
			const unsigned int nTimeSteps=1,
			const T tBias=1.0,
			const bool bTrainable=true);
	Neuron(const Neuron<T>& cSource);
	virtual ~Neuron() {}
	virtual Node& fire(); //not finished
	virtual Node& backPropagate(); //not finished
	Connection<T>& addInput(const Node* pNewIn,
							const char chOperator="*",
							const unsigned int nTimeDelay=0);
	Connection<T>& addOutput(const Node* pNewOut,
							const char chOperator="*",
						  	const unsigned int nTimeDelay=0);
};

#include "Neuron.cpp"

#endif

