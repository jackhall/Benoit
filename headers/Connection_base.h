#ifndef Connection_base_h
#define Connection_base_h

#include <iostream>
#include "NeuralElement.h"
class Node;

class Connection_base : public NeuralElement {
private:
	Connection_base() 
	: NeuralElement(NULL, false, 1),
		mpSource(NULL), mpTarget(NULL) {} //hidden default constructor

protected:
	Node* const mpSource;
	Node* const mpTarget;
	bool mbSignalFlag, mbErrorFlag;
	
	virtual ostream& printElement(ostream& out) const;
	
public:
	Connection_base(Node* pSource,
					Node* pTarget,
			   		const SignalOperand* pSignalOperand,
			   		const unsigned int nTimeSteps=1,
			   		const double dWeight=1.0,
			   		const bool bTrainable=true);
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
