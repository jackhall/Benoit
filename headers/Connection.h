#ifndef Connection_h
#define Connection_h

/*
	Connection is the functional extension of the Connection_base interface, in that it can
	store and process data. It is directly associated with Neuron rather than Neuron_base, because
	Neuron_base, like Connection_base, has no ability to process or store information. Mutual trust
	is necessary for the network to function, and associating Connection to Neuron_base would let a
	programmer write a new child class from Neuron_base that would not work with Connection. 
	
	Separating the neural data structure and interface from the computational implementation leaves 
	the programmer with the possibility of writing completely new implementations of Neuron and 
	Connection without throwing away a perfectly good interface and data structure. 
	
	All the implementation classes in Benoit are templated, but this doesn't mean that they accept any
	data type or class. Currently, they will only function correctly with floating point arithmetic. I
	hope that at some point I or someone else will write a specialization for fixed-point computation. 
	The default for all classes is a float, because the imprecise nature of neural networks essentially
	wastes the information in a double or long double. A float instantiation will use less memory. 
*/

#include <iostream>
#include "Connection_base.h"
#include "State.h"

template<typename T> class Neuron;

template<typename T=float>
class Connection : public Connection_base {
/*
	- Inherits from Connection_base, which provides an interface and network structure. 
	- The default constructor, copy constructor, and assignment operator are all hidden.
		Since a connection is functionally defined by the Nodes it points to, 
		creating a connection with empty pointers or with pointers identical to another
		connection is meaningless. 
	- An integer represents possible recurrent or time-delayed properties. This allows more
		flexible network topologies. The integer is the number of steps a signal is delayed.
	- The connection weight is of the template type. It is public nonconst because it needs
		to be trained and because making it private while providing full public access through
		methods is wasteful. 
	- The operation the Connection performs on passing signals is defined by two function 
		pointers, one for going forward and one for backpropagation. The function pointers
		point to one of several possible functions, which are defined as members of the class.
		This arrangement makes the library harder to maintain but minimizes the overhead for 
		polymorphism. You can't have a pointer to a template, only an instantiation of one. 
*/

private:
	unsigned int mnTimeDelay;

	Connection(); 								//hidden default constructor
	Connection(const Connection<T>& cSource);	//hidden copy constructor
	Connection& operator=(const Connection<T>& cSource);	//hidden assignment operator
	
	//these operate on one sample at a time so that they can by called from the batch
	//operators defined in Neuron
	T add(typename State<T>::iterator iSignal) { return mtWeight+(*iSignal); }		//possible forward operators
	T multiply(typename State<T>::iterator iSignal) { return mtWeight*(*iSignal); }
	T subtract(typename State<T>::iterator iSignal) { return mtWeight-(*iSignal); }
	
	T addD(typename State<T>::iterator iError) { return *iError; }					//possible backward operators
	T multiplyD(typename State<T>::iterator iError) { return (*iError)*mtWeight; }
	T subtractD(typename State<T>::iterator iError) { return -(*iError); }

protected:
	virtual ostream& print(ostream& out) const;
	
public:
	T mtWeight;
	T (*op)(typename State<T>::iterator); 		//will be called from Neuron::fire
	T (*opderiv)(typename State<T>::iterator); 	//will be called from Neuron::backPropagate

	Connection( Neuron<T>* pSource,
				Neuron<T>* pTarget,
			   	const char chOperator='*',			//this allows encapsulation of the operators
			   	const unsigned int nTimeDelay=1,	//and ensures that they match
			   	const T tWeight=0.0,
			   	const bool bTrainable=true);
	Connection( const unsigned int nSource,
				const unsigned int nTarget,
			   	const char chOperator='*',
			   	const unsigned int nTimeDelay=1,
			   	const T tWeight=0.0,
			   	const bool bTrainable=true);
	virtual ~Connection() {}
	void setOperator(const char chOperator);
};

#include "Connection.cpp"

#endif
