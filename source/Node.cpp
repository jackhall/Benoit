#include "Node.h"

unsigned int Node::snID_COUNT = 0;

ostream& Node::print(ostream &out) const {
	
	out << "ID# " << ID << endl;
	return out;
}

Node& Node::addInput(const unsigned int nNewIn) {
	Node* pNewIn = find(nNewIn);
	return addInput(pNewIn);
}

Node& Node::addOutput(const unsigned int nNewOut) {
	Node* pNewOut = find(nNewOut);
	return addOutput(pNewOut);
}

Node& Node::removeInput(const unsigned int nOldIn) {
	Node* pOldIn = find(nOldIn);
	return removeInput(pOldIn);
}

Node& Node::removeOutput(const unsigned int nOldOut) {
	Node* pOldOut = find(nOldOut);
	return removeInput(pOldOut);
}
