#include "Connection.h"

Connection::Connection( int nID,
			const SignalOperand* cpSignalOperand,
			const bool bTimeDelay,
			const double dWeight,
			const bool bTrainable)
	: NeuralElement(cpSignalOperand, false, dWeight, bTrainable),
		mnID(nID), mbSignalFlag(false), mbErrorFlag(false), 
		mbTimeDelay(bTimeDelay)
{}

Connection::~Connection() {

}

ostream& Connection::printElement(ostream& out) const {

	out << "ID# " << mnID;
	
	if(mbTimeDelay) {
		out << ", Time-Delayed" << endl;
	} else {
		out << ", No Time Delay" << endl;
	}
	
	out << NeuralElement::printElement(out);
	
	return out;
}

Connection& Connection::resetSignalFlag() {

	mbSignalFlag = false;
	return *this;
}

Connection& Connection::resetErrorFlag() {
	
	mbErrorFlag = false;
	return *this;
}
