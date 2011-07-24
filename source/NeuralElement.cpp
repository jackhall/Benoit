#include "NeuralElement.h"

using namespace std;
using std::vector;

NeuralElement::NeuralElement(const SignalOperand* pSignalOperand, 
			     const bool bNode,
			     const unsigned int nTimeSteps,
			     const double dWeight, 
			     const bool bTrainable) 
	: mdWeight(dWeight), mbTrainable(bTrainable), mbNode(bNode),
		mpSignalOperand(pSignalOperand), 
		mcSignalState(1, nTimeSteps)
{}


NeuralElement::~NeuralElement() {}

ostream& NeuralElement::printElement(ostream &out) const {
	out << "Signal Operand is " << mpSignalOperand->getName() << endl;
	out << "Weight = " << mdWeight << ", ";
	
	if(mbTrainable) 
		out << "Trainable" << endl;
	else
		out << "Not Trainable" << endl;
	return out;
}

ostream& operator<<(ostream &out, NeuralElement& cNeuralElement) {
	
	out << cNeuralElement.printElement(out);
	return out;
}

