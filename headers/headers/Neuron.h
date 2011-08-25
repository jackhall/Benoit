#ifndef Neuron_h
#define Neuron_h

#include <iostream>
#include "Neuron_base.h"
#include "Connection.h"
#include "ActivationFcn.h"

class Neuron : public Neuron_base {
private:
	Neuron(); //private default constructor
	Neuron& operator=(const Neuron& cSource);

protected:
	const ActivationFcn* mpActivationFcn; //Activation Functions: fLinear, fRBF, fTanh
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	Neuron( const int nID,
			const SignalOperand* pSignalOperand,
			const ActivationFcn* pActivationFcn,
			const unsigned int nTimeSteps=1,
			const double dWeight=1.0,
			const bool bTrainable=true)
		: Neuron_base(nID,pSignalOperand,nTimeSteps,dWeight,bTrainable),
			mpActivationFcn(pActivationFcn) {}
	Neuron(const Neuron& cSource)
		: Neuron_base_h(cSource), 
			mpActivationFcn(cSource.mpActivationFcn) {}
	virtual ~Neuron() {}
	virtual Node& fire();
	virtual Node& backPropagate(); //not finished
	Connection& addInput(const Node* pNewIn,
					 	const SignalOperand* pSignalOperand,
					 	const unsigned int nTimeSteps);
	Connection& addOutput(const Node* pNewOut,
							const SignalOperand* pSignalOperand,
						  	const unsigned int nTimeSteps);
	Node& removeInput(Connection_base* pOldIn);
	Node& removeOutput(Connection_base* pOldOut);
};

#endif