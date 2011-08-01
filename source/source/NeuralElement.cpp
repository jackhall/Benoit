#include "NeuralElement.h"

using namespace std;

NeuralElement::NeuralElement(const SignalOperand* pSignalOperand, 
			     const bool bNode,
			     const unsigned int nTimeSteps,
			     const double dWeight, 
			     const bool bTrainable) 
	: Benoit_base(), mdWeight(dWeight), 
		mbTrainable(bTrainable), mbNode(bNode),
		mpSignalOperand(pSignalOperand), 
		mcSignalState(1, nTimeSteps),
		mcErrorState(1, nTimeSteps)
{}

NeuralElement::NeuralElement(const NeuralElement& cSource) 
	:Benoit_base(), mdWeight(cSource.mdWeight),
		mbTrainable(cSource.mbTrainable), mbNode(cSource.mbNode),
		mpSignalOperand(cSource.mpSignalOperand),
		mcSignalState(cSource.mcSignalState),
		mcErrorState(cSource.mcErrorState)
{}

ostream& NeuralElement::print(ostream &out) const {
	out << "Signal Operand is " << mpSignalOperand->getName() << endl;
	out << "Weight = " << mdWeight << ", ";
	
	if(mbTrainable) 
		out << "Trainable" << endl;
	else
		out << "Not Trainable" << endl;
	return out;
}

