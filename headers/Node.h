#ifndef Node_h
#define Node_h

#include <iostream>
#include <map>
#include "Benoit_base.h"

using namespace std;

class Node : public Benoit_base {
private:
	static unsigned int snID_COUNT;
	static map<unsigned int, Node*> smID_MAP;

	static unsigned int getNewID() { return snID_COUNT++; }
	Node(); //hidden default constructor
	Node& operator=(const Node& cSource);

protected:
	Node(const bool bGroup) 
		: ID(getNewID()), mbGroup(bGroup) {}
	Node(const Node& cSource)
		: ID(getNewID()), mbGroup(cSource.mbGroup) {}
	virtual ostream& print(ostream &out) const; //not finished

public: 
	const unsigned int ID;
	const bool mbGroup;
	
	static inline Node* find(const unsigned int nAddress) 
		{ return smID_MAP[nAddress]; }
	virtual ~Node() {}
	virtual Node& operator++(int) = 0; //advance one time step
	virtual Node& operator--(int) = 0; //go back one time step
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;
	virtual Node& addInput(const Node* pNewIn) = 0; //defined in Neuron?
	Node& addInput(const unsigned int nNewIn);
	virtual Node& addOutput(const Node* pNewOut) = 0; //defined in Neuron?
	Node& addOutput(const unsigned int nNewOut);
	virtual Node& removeInput(Node* pOldIn) = 0;
	Node& removeInput(const unsigned int nOldIn);
	virtual Node& removeOutput(Node* pOldOut) = 0;
	Node& removeOutput(const unsigned int nOldOut);
};

#endif

