#ifndef Node_h
#define Node_h

#include <iostream>
#include "Benoit_base.h"
#include "Connection_base.h"

using namespace std;

class Node : public Benoit_base {
private:
	Node(); //hidden default constructor
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
	Node& operator=(const Node& cSource);

protected:
	Node(const unsigned int nID, const bool bGroup) 
		: mnID(nID), mbGroup(bGroup) {}
	Node(const Node& cSource)
		: mnID(cSource.mnID), mbGroup(cSource.mbGroup) {}
	virtual ostream& print(ostream &out) const; //not finished

public: 
	unsigned int mnID;
	bool isGroup() const { return mbGroup; };
	virtual ~Node() {}
	int getID() const { return mnID; };
	Node& operator++ () { return fire(); };
	Node& operator-- () { return backPropagate(); };
	virtual Node& fire() = 0;
	virtual Node& backPropagate() = 0;
	virtual Connection_base& addInput(	const Node* pNewIn,
					 		  			const SignalOperand* pSignalOperand,
					 		  			const unsigned int nTimeSteps) = 0;
	virtual Connection_base& addOutput( const Node* pNewOut,
								  		const SignalOperand* pSignalOperand,
						  		  		const unsigned int nTimeSteps) = 0;
	virtual Node& removeInput(Node* pOldIn) = 0;
	virtual Node& removeOutput(Node* pOldOut) = 0;
};

#endif
