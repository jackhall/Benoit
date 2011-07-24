#ifndef Node_h
#define Node_h

#include <iostream>
#include <vector>
#include "NeuralElement.h"
class Connection_base;
#include "Connection_base.h"

using namespace std;

class Node {
private:
	Node() : mbGroup(false) {}; //hidden default constructor
	const bool mbGroup;
	friend Connection_base::Connection_base(Node* pSource,
											Node* pTarget,
									   		const SignalOperand* pSignalOperand,
									   		const unsigned int nTimeSteps,
									   		const double dWeight,
									   		const bool bTrainable);
	friend Connection_base::~Connection_base();
	virtual Connection_base& connectInput(Connection_base* const pNew) = 0;
	virtual Connection_base& connectOutput(Connection_base* const pNew) = 0;
	virtual void disconnectInput(const Connection_base* const pOld) = 0;
	virtual void disconnectOutput(const Connection_base* const pOld) = 0;

protected:
	Node(const int nID, const bool bGroup) 
	: mnID(nID), mbGroup(bGroup) {};
	virtual ostream& printNode(ostream &out) const;

public: 
	unsigned int mnID;
	bool isGroup() const { return mbGroup; };
	virtual ~Node() {};
	friend ostream& operator<< (ostream &out, const Node& cNode);
	int getID() const { return mnID; };
	Node& operator++ () { return fire(); };
	Node& operator-- (int) { return backPropagate(); };
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;

};

#endif
