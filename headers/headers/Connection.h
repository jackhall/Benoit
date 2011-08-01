#ifndef Connection_base_h
#define Connection_base_h

#include <iostream>
#include "NeuralElement.h"
#include "Node.h"
#include "SignalOperand.h"

class Connection_base : public NeuralElement {
private:
	Connection_base() 
	: NeuralElement(NULL,false), mbTimeDelay(false) {} //hidden default constructor

protected:
	const Node* mcpSource, mcpTarget;
	bool mbSignalFlag, mbErrorFlag;
	const bool mbTimeDelay;
	
	Connection_base( const bool bTimeDelay,
		   		const SignalOperand* cpSignalOperand,
		   		const double dWeight=1.0,
		   		const bool bTrainable=true);
	virtual ostream& printElement(ostream& out) const;
	
public:
	virtual ~Connection_base();
	bool checkSignalFlag() const { return mbSignalFlag; }
	bool checkErrorFlag() const { return mbErrorFlag; }
	Connection_base& transmitSignal(const State& dSignal);
	Connection_base& transmitError(const State& dError);
	
};

#endif
