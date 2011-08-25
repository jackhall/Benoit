#include "NodeGroup.h"

//(c) Jack Hall 2011, licensed under GNU LGPL v3

NodeGroup::~NodeGroup() {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.end();
	while(!mvNodes.empty()) {
		delete *it;				//cleans up after member Nodes
		mvNodes.erase(--it);
	}
}

Node& NodeGroup::fire() {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.begin();
	vector<Node*>::iterator ite = mvNodes.end();
	for(;it!=ite;++it) {
		(*it)->fire();		//call the fire method for each Node in order
	}
	return *this;
}

Node& NodeGroup::backPropagate(const unsigned int nStepsBack) {
	using namespace std;
	vector<Node*>::iterator it = mvNodes.begin();
	vector<Node*>::iterator ite = mvNodes.end();
	for(;it!=ite;++it) {
		(*it)->backPropagate();		//call the backPropagate method for each Node in order
	}
	return *this;
}

ostream& NodeGroup::print(ostream& out) const {
	Node::print(out);
	//print member nodes?
	return out;
}

