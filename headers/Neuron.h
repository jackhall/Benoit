#ifndef Neuron_h
#define Neuron_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	The Neuron class is the computational core of Benoit. It inherits its interface 
	and network structure from Neuron_base. All signal and error data is stored in
	Neurons, and Neurons perform or initiate all neural computation. This separation
	of network structure and interface from implementation will hopefully make Benoit
	more flexible and extensible.
	
	Neuron, like Connection and other non-interface classes in Benoit, is templated for
	any floating-point arithmetic. Because neural networks have inherent uncertainty, the 
	default template type is a float so as not to waste memory with useless precision. I hope 
	in the future to implement a fixed-point specialization of Benoit's implementation
	classes. 
	
	Neuron is tightly tied to Connection, as described in that class's documentation. 
	Connection is relied upon to have a certain set of function pointers and data members
	that intermingle with members of Neuron during execution. 
*/

#include <iostream>
#include "Neuron_base.h"
#include "Connection.h"
#include "State.h"

template<typename T=float>
class Neuron : public Neuron_base {
/*
	- Inherits from Neuron_base, which defines the network structure and interface. Like 
		Neuron_base and Node, the assignment operator is hidden but not the copy constructor.
	- Completes network management features with methods to create Connections.
	- Defines the mid-level computations done by a neural network in fire and backPropagate. 
		Low-level computations are implemented in the State and State::iterator classes. 
	- A neural bias is implemented as a data member of the template type. 
	- Signal and error data is stored in four State members (circular arrays of arrays). 
		Signal: The result of combining all inputs from input Connections. It stores a 
				time history of all samples for a certain number of time steps. Needed to 
				evaluate activation function derivatives during backpropagation through time.
		Output:	The result of the forward activation function, stored similarly to signals. 
				Neurons that take the current Neuron as an input draw data from this member.
		Error:	The current error derivative, sampled by Neurons for which the current Neuron
				is an output. No time history is kept, but all samples are saved separately.
		Buffer:	The result of combining the errors from all output Neurons. Does not keep a
				time history.
	- Two function pointers specify how input signals are combined and how to distribute
		derivatives. Similar to those in Connection, but operating on a batch of samples rather
		than one sample at a time. The two must matched, and access methods/constructors ensure this.
	- Two function pointers specifying the activation function of the Neuron and its derivative. 
		So far only three are defined: tanh (a sigmoid), linear, and exp(-x^2) (a radial basis function).
		Like the operator functions, access functions and the constructors ensure they match. 
*/

private:
	Neuron(); 			//hidden default constructor
	Neuron& operator=(const Neuron& cSource);	//hidden assignment operator
	
	State<T> msSignal, msOutput, msError, msBuffer;		//signal and error data containers

protected:
	virtual ostream& print(ostream &out) const; //not finished
	
public:
	T mtBias;
	Neuron(	const unsigned int nSamples=1,		//size of batch
			const unsigned int nTimeSteps=1,	//number of time steps to backpropagate through
			const T tBias=0.0,
			const bool bTrainable=true)
		: Neuron_base(bTrainable), mtBias(tBias), 
			msSignal(nSamples, nTimeSteps), msOutput(nSamples, nTimeSteps),
			msError(nSamples), msBuffer(nSamples) {}
	Neuron(const Neuron<T>& cSource);
	virtual ~Neuron() {}
	virtual Node& fire() = 0; 			//virtual from Node
	virtual Node& backPropagate(const unsigned int nStepsBack=0) = 0; //virtual from Node
	Node& addInput( const Node* pNewIn,
					const char chOperator="*",
					const unsigned int nTimeDelay=0);	//virtual from Node, not defined in Neuron_base
	Node& addOutput(const Node* pNewOut,				//because it needs to instantiate Connections
					const char chOperator="*",
					const unsigned int nTimeDelay=0);	//virtual from Node
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
template<typename T>
Node& Neuron<T>::addInput(	const Node* pNewIn,
							const char chOperator,
							const unsigned int nTimeDelay) {
	if(pNewIn->mbGroup) pNewIn->addOutput(this, chOperator, nTimeDelay); //allow NodeGroup to create needed objects
	else new Connection<T>(pNewIn, this, chOperator, nTimeDelay);	//create a single Connection (not Connection_base)
	return *this;
}

template<typename T>
Node& Neuron<T>::addOutput(	const Node* pNewOut,
							const char chOperator,
						  	const unsigned int nTimeDelay) {
	if(pNewOut->mbGroup) pNewOut->addInput(this, chOperator, nTimeDelay); //allow NodeGroup to create needed objects
	else new Connection<T>(this, pNewOut, chOperator, nTimeDelay);	//create a single Connection (not Connection_base)
	return *this;
}

template<typename T>
ostream& Neuron<T>::print(ostream &out) const {
	//weight, call Neuron_base::print
	//state data?
	return out;
}

#endif

