//(c) Jack Hall 2011, licensed under GNU LGPL v3

#include "Node.h"

unsigned int Node::snID_COUNT = 0; //initialize ID numbers

ostream& Node::print(ostream &out) const {
	//more here?
	out << "ID# " << ID << endl;
	return out;
}

Node& Node::addInput(const unsigned int nNewIn,
					 const char chOperator,
					 const unsigned int nTimeDelay) {
	//look up Node ID
	Node* pNewIn = find(nNewIn);
	//call a Connection_base-derived constructor
	return addInput(pNewIn, chOperator, nTimeDelay); //defined in child classes
}

Node& Node::addOutput(	const unsigned int nNewOut,
						const char chOperator,
						const unsigned int nTimeDelay) {
	//look up Node ID
	Node* pNewOut = find(nNewOut);
	//call a Connection_base-derived constructor
	return addOutput(pNewOut, chOperator, nTimeDelay); //defined in child classes
}

Node& Node::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	Node* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn); //defined in child classes
}

Node& Node::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	Node* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput(pOldOut); //defined in child classes
}
