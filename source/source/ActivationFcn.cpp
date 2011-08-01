#include "ActivationFcn.h"

//fLinear methods
State& fLinear::eval(const State& cSignal, State& cOutput) const {
	for(unsigned int i=cSignal.samples()-1;i>=0;--i) {
		cOutput(i) = cSignal(i);
	}
	return cOutput;
}

State& fLinear::evalDerivative(const State& cSignal, State& cOutput) const {
	cOutput.initializeStep(1.0);
	return cOutput;
}

//fRBF methods
State& fRBF::eval(const State& cSignal, State& cOutput) const {
	for(unsigned int i=cSignal.samples()-1;i>=0;--i) {
		cOutput(i) = exp(-(cSignal(i)^2.0));
	}
	return cOutput;
}

State& fRBF::evalDerivative(const State& cSignal, State& cOutput) const {
	for(unsigned int i=cSignal.samples()-1;i>=0;--i) {
		cOutput(i) = -2.0*cSignal(i)*exp(-(cSignal(i)^2.0));
	}
	return cOutput;
}

//fTanh methods
State& fTanh::eval(const State& cSignal, State& cOutput) const {
	for(unsigned int i=cSignal.samples()-1;i>=0;--i) {
		cOutput(i) = tanh(cSignal(i));
	}
	return cOutput;
}

State& fTanh::evalDerivative(const State& cSignal, State& cOutput) const {
	for(unsigned int i=cSignal.samples()-1;i>=0;--i) {
		cOutput(i) = (1.0/cosh(cSignal(i)))^2.0;
	}
	return cOutput;
}
