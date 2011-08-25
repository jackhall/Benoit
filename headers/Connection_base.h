#ifndef Connection_base_h
#define Connection_base_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	Connection_base is the interface class for all connection between Nodes. 
	Since connections are causally dependent, all calculations are initiated from 
	the Nodes they are connected to. The constructors and destructors of Connection_base
	are friended by Neuron_base. In this way a Connection_base can create its own two-way 
	Node interfaces on construction and clean them up on destruction. Any other 
	changes are disallowed because they would leave hanging pointers. 
	
	Connection_base defines no computations or data members in order to help separate
	the data structure and interface of Benoit from its implementation. This will 
	help make the library more extensible and flexible. 
*/

#include <iostream>
#include "Benoit_base.h"
class Neuron_base; 			//Neuron_base.h is included in the .cpp file

class Connection_base : public Benoit_base {
/*
	- Inherits from Benoit_base for external interface purposes.
	- The main resources handled by Connection_base are pointers to the Nodes being connected. 
		These Nodes are Neuron_bases because this ensures that the Nodes do not hold others. 
	- The default constructor, copy constructor, and assignment operator are all hidden.
		Since a connection is functionally defined by the Nodes it points to, 
		creating a connection with empty pointers or with pointers identical to another
		connection is meaningless. 
	- A boolean enables or disables the ability to change the weight or other function of the 
		connection by training. 
	- Two other booleans represent the causal dependence of the Connection_base on its Nodes. 
		They are set or reset as Nodes fire. 
	- Methods provide access for the Node pointers to each other. 
*/

private:
	Connection_base(); 			//hidden default constructor
	Connection_base& operator=(const Connection_base& cSource); //hidden assignment operator
	Connection_base(const Connection_base& cSource); 			//hidden copy constructor

protected:
	Neuron_base* const mpSource;	//You cannot change the fundamental properties of a connections
	Neuron_base* const mpTarget;	//once it is created. This prevents semantic mistakes.
	
	virtual ostream& print(ostream& out) const; //not finished
	
public:
	bool mbTrainable; 				//used only by child classes
	bool mbSignalFlag, mbErrorFlag;	
	
	inline Neuron_base* getSource() { return mpSource; }
	inline Neuron_base* getTarget() { return mpTarget; }
	Connection_base(Neuron_base* pSource,
					Neuron_base* pTarget,
			   		const bool bTrainable=true);
	Connection_base(const unsigned int nSource, 	//these integers
					const unsigned int nTarget,		//are Node addresses
					const bool bTrainable=true);
	virtual ~Connection_base();
};

#endif

