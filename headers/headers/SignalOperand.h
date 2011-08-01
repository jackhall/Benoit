#ifndef SignalOperand_h
#define SignalOperand_h

#include <string>
#include "State.h"

using namespace std;

class SignalOperand {
private:
	string mstrName;
	
protected:
	SignalOperand(string strName) : mstrName(strName) {}
	
public:
	string getName() const { return mstrName; }
	virtual State& eval(State& cTarget, const State& cNew) const = 0;
	virtual State& evalDerivative(State& cTarget, const State& cNew) const = 0;
};

class OpAdd : public SignalOperand {
public:
	OpAdd() : SignalOperand("add") {}
	State& eval(State& cTarget, const State& cNew) const;
	State& evalDerivative(State& cTarget, const State& cNew) const;
};

class OpMultiply : public SignalOperand {
public:
	State& eval(State& cTarget, const State& cNew) const;
	State& evalDerivative(State& cTarget, const State& cNew) const;
};

#endif
