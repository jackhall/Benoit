#include "SignalOperand.h"

//OpAdd methods
double OpAdd::eval(const double dWeight, const vector<double> vdIn) const {
	double dOutput = dWeight;
	for(int i=vdIn.size()-1; i>=0; i--) {
		dOutput += vdIn[i];
	}
	return dOutput;
}

double OpAdd::eval(const double dWeight, const double dIn) const {
	return dWeight + dIn;
}

vector<double> OpAdd::evalDerivative(const double dWeight, const vector<double> vdIn) const {
	vector<double> vdOut(vdIn.size(),1.0);
	return vdOut;
}

double OpAdd::evalDerivative(const double dWeight, const double dIn) const {
	return 1.0;
}

//OpMultiply methods
double OpMultiply::eval(const double dWeight, const vector<double> vdIn) const {
	double dOutput = dWeight;
	for(int i=vdIn.size()-1; i>=0; i--) {
		dOutput = dOutput * vdIn[i];
	}
	return dOutput;
}

double OpMultiply::eval(const double dWeight, const double dIn) const {
	return dWeight * dIn;
}

vector<double> OpMultiply::evalDerivative(const double dWeight, const vector<double> vdIn) const {
	double dTotalProduct = eval(dWeight,vdIn);
	unsigned int n = vdIn.size();
	vector<double> vdOut(n,0.0);
	for(int i=n-1; i>=0; i--) {
		vdOut[i] = dTotalProduct / vdIn[i];
	}
	return vdOut;
}

double OpMultiply::evalDerivative(const double dWeight, const double dIn) const {
	return dWeight;
}

