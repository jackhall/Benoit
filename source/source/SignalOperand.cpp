#include "SignalOperand.h"

//OpAdd methods
State& OpAdd::eval(State& cTarget, const State& cNew) const {
	for(unsigned int i=cTarget.samples()-1;i>=0;--i) {
		cTarget(i) += cNew(i);
	}
	return cTarget;
}

State& OpAdd::evalDerivative(State& cTarget, const State& cNew) const {
	return cTarget;
}

//OpMultiply methods
State& OpMultiply::eval(State& cTarget, const State& cNew) const {
	for(unsigned int i=cTarget.samples()-1;i>=0;--i) {
		cTarget(i) = cTarget(i) * cNew(i);
	}
	return cTarget;
}

State& OpMultiply::evalDerivative(State& cTarget, const State& cNew) const {
	return cTarget;
}
