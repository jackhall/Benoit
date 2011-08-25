#ifndef NeuralElement_h
#define NeuralElement_h

#include <cstdlib>
#include <iostream>
#include "Benoit_base.h"
#include "SignalOperand.h"
#include "State.h"

using namespace std;

class NeuralElement : public Benoit_base {
private:
	NeuralElement(); //hidden default constructor
	NeuralElement& operator=(NeuralElement& cSource);
	
protected:
	State mcSignalState, mcErrorState;
	const SignalOperand* mpSignalOperand; //Signal Operands: OpAdd, OpMultiply
	
	NeuralElement(	const SignalOperand* cpSignalOperand, 
					const bool bNode,
					const unsigned int nTimeSteps,
					const double dWeight=1.0, 
					const bool bTrainable=true);
	NeuralElement(const NeuralElement& cSource);
	virtual ostream& print(ostream& out) const;
	
public:
	const bool mbNode;
	double mdWeight;
	bool mbTrainable;
	virtual ~NeuralElement() {}
};

#endif