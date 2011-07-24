#include "ActivationFcn.h"

//fLinear methods
double fLinear::eval(const double dSignal) const {
	return dSignal;
}

double fLinear::evalDerivative(const double dError) const {
	return 1.0;
}

//fRBF methods
double fRBF::eval(const double dSignal) const {
	return exp(-(dSignal^2.0));
}

double fRBF::evalDerivative(const double dError) const {
	return return -2.0*dSignal*exp(-(dSignal^2.0));
}

//fTanh methods
double fTanh::eval(const double dSignal) const {
	return tanh(dSignal);
}

double fTanh::evalDerivative(const double dSignal) const {
	return (1.0/cosh(dSignal))^2.0;
}
