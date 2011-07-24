#include "Neuron.h"

Neuron::Neuron(	const int nID,
	       	const SignalOperand* cpSignalOperand,
	       	const ActivationFcn* cpActivationFcn,
	   	const double dWeight,
	   	const bool bTrainable)
	: NeuralElement(cpSignalOperand, true, dWeight, bTrainable),
		Node(nID),
		mcpActivationFcn(cpActivationFcn)
{}

Neuron::~Neuron() { 
	
	deleteConnections(mqpConnIn);
	deleteConnections(mqpConnOut);
}

Neuron& Neuron::fire() {

	vector<double> vdIn = collectSignals();
	
	mdSignalState = mcpSignalOperand->eval(mdWeight, vdIn);

	double dOutput = mcpActivationFcn->eval(mdSignalState);
	
	distributeOutput(dOutput);
	return *this;
}

Neuron& Neuron::backPropagate() {

	double dTotalError = collectErrors();
	vector<double> vdIn = collectSignals();
	
	mdErrorState = dTotalError * mcpActivationFcn->evalDerivative(mdSignalState);
	
	vector<double> vdBack = mcpSignalOperand->evalDerivative(mdWeight, vdIn);
	vector<double>::iterator it = vdBack.begin();
	vector<double>::iterator ite = vdBack.end();
	while(it != ite) {
		*it = *it * mdErrorState;
		it++;
	}
	
	distributeError(vdBack);
	return *this;
}

vector<double> Neuron::collectSignals() const {
	int n = mqnConnIn.size();
	vector<double> vdIn(n);
	map<int, Connection*>& ConnMap = *mpConnMap;
	
	for(int i=n-1; i>=0; i--) {	//update syntax to use iterators
		assert( ConnMap[ mqnConnIn[i] ]->checkSignalFlag() );
		vdIn[i] = ConnMap[ mqnConnIn[i] ]->getSignal();
	}
	
	return vdIn;
}

void Neuron::distributeOutput(const double dOutput) const {
	map<int, Connection*>& ConnMap = *mpConnMap;

	for(int i=mqnConnOut.size()-1; i>=0; i--) {//update syntax to use iterators
		ConnMap[ mqnConnOut[i] ]->transmitSignal(dOutput);
	}
	
	return;
}

double Neuron::collectErrors() const {
	double dTotalError = 0.0;
	map<int, Connection*>& ConnMap = *mpConnMap;
	
	for(int i=mqnConnOut.size()-1; i>=0; i--) { //update syntax to use iterators
		assert( ConnMap[ mqnConnOut[i] ]->checkErrorFlag() );
		dTotalError += ConnMap[ mqnConnOut[i] ]->getError();
	}
	return dTotalError;
}

void Neuron::distributeError(const vector<double> vdBack) const {
	map<int, Connection*>& ConnMap = *mpConnMap;

	for(int i=vdBack.size()-1; i>=0; i--) {	//update syntax to use iterators
		ConnMap[ mqnConnIn[i] ]->transmitError(vdBack[i]);
	}
	return;
}

ostream& Neuron::printNode(ostream &out) const {
	out << Node::printNode(out);
	out << "Activation Function is " << mcpActivationFcn->getName() << endl; 
	out << NeuralElement::printElement(out);
	//print connections?
	
	return out;
}

ostream& Neuron::printElement(ostream& out) const {

	out << printNode(out);
	return out;
}
