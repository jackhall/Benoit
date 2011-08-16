#include "Neuron_base.h"

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

Node& Neuron_base::removeInput(Node* pOldIn) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(pOldIn == (*it)->getSource()) {
			delete *it;
			break;
		}
		++it;
	} 
	return *this;
}

Node& Neuron_base::removeOutput(Node* pOldOut) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		if(pOldOut == (*it)->getTarget()) {
			delete *it;
			break;
		}
		++it;
	}
	return *this;
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
	Node::print(out);
	return out;
}

