#ifndef Connection_base_h
#define Connection_base_h

#include <iostream>
#include "NeuralElement.h"
class Node;
class Neuron_base;

class Connection_base : public NeuralElement {
private:
	Connection_base();  //hidden default constructor
	Connection_base& operator=(const Connection_base& cSource);
	Connection_base(const Connection_base& cSource);

protected:
	Node* const mpSource;
	Node* const mpTarget;
	bool mbSignalFlag, mbErrorFlag;
	
	virtual ostream& print(ostream& out) const; //not finished
	friend class Neuron_base; //because friend functions cause circularity
	
public:
	Connection_base(Node* pSource,
					Node* pTarget,
			   		const SignalOperand* pSignalOperand,
			   		const unsigned int nTimeSteps=1,
			   		const double dWeight=1.0,
			   		const bool bTrainable=true);
	virtual ~Connection_base();
	bool checkSignalFlag() const { return mbSignalFlag; }
	bool checkErrorFlag() const { return mbErrorFlag; }
	Connection_base& resetSignalFlag();
	Connection_base& resetErrorFlag();
	const State& getSignal() const { return mcSignalState; }
	const State& getError() const { return mcErrorState; }
	virtual Connection_base& transmitSignal(const State& dSignal) = 0;
	virtual Connection_base& transmitError(const State& dError) = 0;
	
};

#endif