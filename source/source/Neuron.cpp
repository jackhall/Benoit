#include "Neuron.h"

Node& Neuron::fire() {
	collectSignals();
	mpActivationFcn->eval(mcSignalState, mcOutputBuffer);
	distributeOutput();
	return *this;
}

Node& Neuron::backPropagate() { //not finished

	collectErrors();
	//evaluate derivatives
	distributeError();
	return *this;
}

ostream& Neuron::print(ostream &out) const {
	out << "Activation Function is " << mcpActivationFcn->getName() << endl; 
	Neuron_base::print(out);
	//print connections?
	
	return out;
}

