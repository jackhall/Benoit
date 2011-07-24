#ifndef NodeGroup_h
#define NodeGroup_h

class NodeGroup : public Node {
protected:
	//NODE static unsigned int mnNewID;
	void addNode(const vector<unsigned int> nLocus, const Node cNewNode);
	void pruneNode(const vector<unsigned int> nLocus);
	
public:
	//IOUNIT virtual double[] operator>>(const double[],IOUnit) = 0; //overloaded input operator
	//NODE const unsigned int nID;
	//NODE int operator>> (Node &cNode);
	//NODE int operator<< (Node &cNode, int);

}

#endif