#ifndef Ganglion_h
#define Ganglion_h

#include <vector>

class Ganglion : public Layer {
protected:
	//NODE static unsigned int mnNewID;
	//NODEGROUP void addNode(const vector<unsigned int> nLocus, const Node cNewNode);
	//NODEGROUP void pruneNode(const vector<unsigned int> nLocus);
	vector<Neuron> mvcNeurons;
	
public:
	//IOUNIT virtual double[] operator>>(const double[],IOUnit) = 0; //overloaded input operator
	//NODE const unsigned int nID;
	//NODE int operator>> (Node &cNode);
	//NODE int operator<< (Node &cNode, int);

}

#endif
