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
	Node& removeNode(Node* const pOld);
	virtual ostream& print(ostream& out) const; //not finished
	
public:
	NodeGroup() : Node(0,true), mvNodes() {}
	NodeGroup(const unsigned int nID) : Node(nID,true), mvNodes() {}
	NodeGroup(const unsigned int nID, const vector<Node*> vNodes)
		: Node(nID,true), mvNodes(vNodes) {}
	virtual ~NodeGroup();
	virtual Node& fire();
	virtual Node& backPropagate();
};

#endif
