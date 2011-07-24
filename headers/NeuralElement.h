#ifndef NeuralElement_h
#define NeuralElement_h

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "SignalOperand.h"
#include "State.h"

using namespace std;
using std::vector;

class NeuralElement {
private:
	NeuralElement() : mbNode(false) {} //hidden default constructor
	
protected:
	State mcSignalState, mcErrorState;
	const SignalOperand* mpSignalOperand; //Signal Operands: OpAdd, OpMultiply
	
	NeuralElement(	const SignalOperand* cpSignalOperand, 
					const bool bNode,
					const unsigned int nTimeSteps,
					const double dWeight=1.0, 
					const bool bTrainable=true);
	virtual ostream& printElement(ostream& out) const;
	
public:
	const bool mbNode;
	double mdWeight;
	bool mbTrainable;
	virtual ~NeuralElement() = 0;
	friend ostream& operator<<(ostream &out, NeuralElement& cNeuralElement);
};

#endif
