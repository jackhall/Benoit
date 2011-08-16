#ifndef NodeGroup_h
#define NodeGroup_h

#include <vector>
#include "Node.h"

class NodeGroup : public Node {
private:
	NodeGroup& operator=(const NodeGroup& cSource);
	NodeGroup(const NodeGroup& cSource);
	
protected:
	vector<Node*> mvNodes;
	Node& addNode(Node* const pNew);
	Node& addNode(const unsigned int nNew);
	Node& removeNode(Node* const pOld);
	Node& removeNode(const unsigned int nOld);
	virtual ostream& print(ostream& out) const; //not finished
	
public:
	NodeGroup() : Node(true), mvNodes() {}
	NodeGroup(const vector<Node*> vNodes)
		: Node(true), mvNodes(vNodes) {}
	virtual ~NodeGroup();
	virtual Node& fire();
	virtual Node& backPropagate();
};

#endif
