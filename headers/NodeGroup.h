#ifndef NodeGroup_h
#define NodeGroup_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	NodeGroup is the interface for organizing Nodes. This can serve either to hide implementation or 
	to simply make handling neural structures more convenient. 
*/

#include <vector>
#include "Node.h"

class NodeGroup : public Node {
/*
	- Inherits causal independence and interface from Node.
	- Unlike Node, both copy constructing and assignment are disallowed. Change in future?
	- Access functions for adding and removing Nodes are protected because nothing is instantiated.
	- Node pointers are stored in a vector. Pointers allow different children of Node to coexist. 
	- fire and backPropagate just call themselves for each Node in the vector. 
*/

private:
	NodeGroup& operator=(const NodeGroup& cSource);
	NodeGroup(const NodeGroup& cSource);
	
protected:
	vector<Node*> mvNodes;
	
	Node& addNode(Node* const pNew);			//not defined yet
	Node& addNode(const unsigned int nNew); 	//not defined yet
	Node& removeNode(Node* const pOld);			//not defined yet
	Node& removeNode(const unsigned int nOld);	//not defined yet
	virtual ostream& print(ostream& out) const; //not finished
	
public:
	NodeGroup() : Node(true), mvNodes() {}
	NodeGroup(const vector<Node*> vNodes)
		: Node(true), mvNodes(vNodes) {}
	virtual ~NodeGroup();
	virtual Node& fire();
	virtual Node& backPropagate(const unsigned int nStepsBack);
};

#endif
