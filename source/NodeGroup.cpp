#include "NodeGroup.h"

NodeGroup::~NodeGroup() {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.end();
	while(!mvNodes.empty()) {
		delete *it;
		mvNodes.erase(--it);
	}
}

Node& NodeGroup::fire() {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.begin();
	vector<Node*>::iterator ite = mvNodes.end();
	for(;it!=ite;++it) {
		(*it)->fire();
	}
	return *this;
}

Node& NodeGroup::backPropagate() {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.begin();
	vector<Node*>::iterator ite = mvNodes.end();
	for(;it!=ite;++it) {
		(*it)->backPropagate();
	}
	return *this;
}

ostream& NodeGroup::print(ostream& out) const {
	Node::print(out);
	return out;
}

