#include "Connection_base.h"
#include "Node.h"

Connection_base::Connection_base(	Node* pSource,
									Node* pTarget,
							   		const SignalOperand* pSignalOperand,
							   		const unsigned int nTimeSteps,
							   		const double dWeight,
							   		const bool bTrainable)
	: NeuralElement(pSignalOperand, false, nTimeSteps, dWeight, bTrainable),
		mpSource(pSource), mpTarget(pTarget),
		mbSignalFlag(false), mbErrorFlag(false) {
	mpTarget->connectInput(this);
	mpSource->connectOutput(this);
}

Connection_base::~Connection_base() {
	mpTarget->disconnectInput(this);
	mpSource->disconnectOutput(this);
}

ostream& Connection_base::printElement(ostream& out) const {
	
	out << NeuralElement::printElement(out);
	return out;
}

Connection_base& Connection_base::resetSignalFlag() {

	mbSignalFlag = false;
	return *this;
}

Connection_base& Connection_base::resetErrorFlag() {
	
	mbErrorFlag = false;
	return *this;
}
