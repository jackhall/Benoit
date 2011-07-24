#include "Neuron_base.h"

Neuron_base::Neuron_base(const int nID,
						 const SignalOperand* pSignalOperand,
						 const unsigned int nTimeSteps,
						 const double dWeight,
						 const bool bTrainable)
	: NeuralElement(pSignalOperand, true, nTimeSteps, dWeight, bTrainable), 
		Node(nID, false) 
{}

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
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(*it==pOld) {
			mqConnIn.erase(it);
			break;
		}
	}
}

void Neuron_base::disconnectOutput(const Connection_base* const pOld) {
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		if(*it==pOld) {
			mqConnOut.erase(it);
			break;
		}
	}
}

void Neuron_base::deleteConnections(deque<Connection_base*>& qConnections) {
	
	deque<Connection_base*>::iterator it = qConnections.begin();
	
	while(!qConnections.empty()) {
		delete *it;
		qConnections.erase(it++);
	}
	return;
}

Neuron_base& Neuron_base::removeInput(Connection_base* pOldIn) {

	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(pOldIn == *it) {
			delete *it;
			break;
		}
		it++;
	}
	return *this;
}

Neuron_base& Neuron_base::removeOutput(Connection_base* pOldOut) {

	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		if(pOldOut == *it) {
			delete *it;
			break;
		}
		it++;
	}
	 
	return *this;
}

