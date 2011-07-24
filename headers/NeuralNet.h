#ifndef NeuralNet_h
#define NeuralNet_h

#include <vector>

class NeuralNet : public NodeGroup {
protected:
	//NODE static unsigned int mnNewID;
	//NODEGROUP void addNode(const vector<unsigned int> nLocus, const Node cNewNode);
	//NODEGROUP void pruneNode(const vector<unsigned int> nLocus);
	vector<Connection> mvcConnections;
	vector<vector<Node>> mvcNodes; //first vector is layer, second vector is node
	unsigned int findNode(const vector<unsigned int> vnLocus);
	unsigned int findSynapse(const unsigned int nNodeOutID, const unsigned int nNodeInID);
	
public:
	//IOUNIT virtual double[] operator>>(const double[],IOUnit) = 0; //overloaded input operator
	//NODE const unsigned int nID;
	//NODE int operator>> (Node &cNode);
	//NODE int operator<< (Node &cNode, int);
	void addConnection(const vector<unsigned int> nOriginLocus, const vector<unsigned int> nEndLocus);
	void pruneConnection(const vector<unsigned int> nOriginLocus, const vector<unsigned int> nEndLocus);
		
}

#endif