#include "Node.h"

ostream& operator<< (ostream &out, const Node& cNode) {
	
	out << cNode.printNode(out);
	return out;
}

ostream& Node::printNode(ostream &out) const {
	
	out << "ID# " << mnID << endl;
	return out;
}
