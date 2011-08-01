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
	Connection_base& resetSignalFlag();
	Connection_base& resetErrorFlag();
	virtual double getSignal() = 0;
	virtual double getError() = 0;
	virtual Connection_base& transmitSignal(double dSignal) = 0;
	virtual Connection_base& transmitError(double dError) = 0;
	
};

#endif
