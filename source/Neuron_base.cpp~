#include "Neuron_base.h"

Neuron_base::Neuron_base(const int nID,
						 const SignalOperand* pSignalOperand,
						 const unsigned int nTimeSteps,
						 const double dWeight,
						 const bool bTrainable)
	: NeuralElement(pSignalOperand, true, nTimeSteps, dWeight, bTrainable), 
		Node(nID, false)
{}

Neuron_base::Neuron_base(const Neuron_base& cSource) 
	: NeuralElement(cSource), Node(cSource)
{}

Neuron_base::~Neuron_base() {
	deleteConnections(mqConnIn);
	deleteConnections(mqConnOut);
}

Connection_base& Neuron_base::connectInput(Connection_base* const pNew) {
	Connection_base* pNewTemp = pNew; 
	mqConnIn.push_back(pNewTemp); 
	return *pNew;
}
		
Connection_base& Neuron_base::connectOutput(Connection_base* const pNew) { 
	Connection_base* pNewTemp = pNew;
	mqConnOut.push_back(pNewTemp);
	return *pNew; 
}
		
void Neuron_base::disconnectInput(const Connection_base* const pOld) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(*it==pOld) {
			mqConnIn.erase(it);
			break;
		}
		++it;
	}
}

void Neuron_base::disconnectOutput(const Connection_base* const pOld) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		if(*it==pOld) {
			mqConnOut.erase(it);
			break;
		}
		++it;
	}
}

void Neuron_base::deleteConnections(deque<Connection_base*>& qConnections) {
	using namespace std;
	deque<Connection_base*>::iterator it = qConnections.end();
	
	while(!qConnections.empty()) {
		delete *it;
		qConnections.erase(--it);
	}
	return;
}

State& Neuron_base::collectSignals() {
	++mcSignalState.initializeStep(mdWeight);
	State* pConnectionState = NULL;
	for(unsigned int i=mqConnIn.size()-1; i>=0; --i) {	
		mpSignalOperand->eval(mcSignalState,mqConnIn[i]->mcSignalState);
		mqConnIn[i]->mbSignalFlag = false;
	}
	return mcSignalState;
}

State& Neuron_base::collectErrors() {
	mcOutputBuffer.initializeStep(0.0);
	State* pConnectionState = NULL;
	for(int i=mqConnOut.size()-1; i>=0; --i) { 
		mcOutputBuffer += mqConnOut[i]->mcErrorState;
		mqConnOut[i]->mbErrorFlag = false;
	}
	return mcOutputBuffer;
}

void Neuron_base::distributeOutput() const {
	for(int i=mqConnOut.size()-1; i>=0; --i) {
		mqConnOut[i]->transmitSignal(mcOutputBuffer);
	}	
	return;
}

void Neuron_base::distributeError() const {
	for(int i=mqConnIn.size()-1; i>=0; --i) {
		mqConnIn[i]->transmitError(mcErrorState);
	}	
	return;
}

Neuron_base& Neuron_base::removeInput(Connection_base* pOldIn) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(pOldIn == *it) {
			delete *it;
			break;
		}
		++it;
	}
	return *this;
}

Neuron_base& Neuron_base::removeOutput(Connection_base* pOldOut) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		if(pOldOut == *it) {
			delete *it;
			break;
		}
		++it;
	}
	return *this;
}

ostream& Neuron_base::print(ostream& out) const {
	NeuralElement::print(out);
	Node::print(out);
	return out;
}
