#include "Node.h"

ostream& Node::print(ostream &out) const {
	
	out << "ID# " << mnID << endl;
	return out;
}
