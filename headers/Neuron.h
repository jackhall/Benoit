#ifndef Neuron_h
#define Neuron_h

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <assert.h>
#include "Neuron_base.h"
#include "Connection.h"
#include "ActivationFcn.h"
#include "State.h"

class Neuron : public Neuron_base {
private:
	Neuron() 
	: NeuralElement(NULL,true), Node(0.0) {} //private default constructor

protected:
	const ActivationFcn* mpActivationFcn; //Activation Functions: fLinear, fRBF, fTanh
	
	virtual ostream& printNode(ostream &out) const;
	virtual ostream& printElement(ostream& out) const;
	Matrix collectSignals() const;
	void distributeOutput(const Matrix& dOutput) const;
	Matrix collectErrors() const;
	void distributeError(const Matrix& vdBack) const;
	
public:
	Neuron( const int nID,
			const SignalOperand* cpSignalOperand,
			const ActivationFcn* cpActivationFcn,
			const double dWeight=1.0,
			const bool bTrainable=true);
	virtual ~Neuron();
	virtual Neuron& fire();
	virtual Neuron& backPropagate();
};

#endif
