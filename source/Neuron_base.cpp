//(c) Jack Hall 2011, licensed under GNU LGPL v3

#include "Neuron_base.h"

Neuron_base::~Neuron_base() {
	deleteConnections(mqConnIn);		//deletes all elements of the deque and deconstructs 
	deleteConnections(mqConnOut);		//all corresponding Connection_bases
}

Connection_base& Neuron_base::connectInput(Connection_base* const pNew) {
	Connection_base* pNewTemp = pNew; 
	mqConnIn.push_back(pNewTemp); 		//adds existing Connection object
	return *pNew;
}
	
Connection_base& Neuron_base::connectOutput(Connection_base* const pNew) { 
	Connection_base* pNewTemp = pNew;
	mqConnOut.push_back(pNewTemp);		//adds existing Connection_base
	return *pNew; 
}

void Neuron_base::disconnectInput(const Connection_base* const pOld) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnIn.begin(); 
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(*it==pOld) {
			mqConnIn.erase(it);			//does not deconstruct Connection_base!
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
			mqConnOut.erase(it);		//does not deconstruct Connection_base!
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
		qConnections.erase(--it);		//deconstructs and removes all Connection_bases from the deque
	}
	return;
}

Node& Neuron_base::removeInput(Node* pOldIn) {
	using namespace std;
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		if(pOldIn == (*it)->getSource()) {	//if the Connection_base's input matches the node given
			delete *it;						//deconstruct Connection_base, it removes itself from the deque
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
		if(pOldOut == (*it)->getTarget()) {	//if the Connection_base's input matches the node given
			delete *it;						//deconstruct Connection_base, it removes itself from the deque
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
		if(pOldIn == *it) {		//if the Connection_base pointer matches the input argument
			delete *it;			//deconstruct Connection_base, it removes itself from the deque
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
		if(pOldOut == *it) {	//if the Connection_base pointer matches the input argument
			delete *it;			//deconstruct Connection_base, it removes itself from the deque
			break;
		}
		++it;
	}
	return *this;
}

ostream& Neuron_base::print(ostream& out) const {
	Node::print(out);
	//print connections?
	return out;
}

