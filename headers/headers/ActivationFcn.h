#ifndef ActivationFcn_h
#define ActivationFcn_h

#include <string>
#include <math.h>

using namespace std;

class ActivationFcn {
private:
	string mstrName;
	
protected:
	ActivationFcn(string strName) : mstrName(strName) {}
	
public:
	string getName() const { return mstrName; }
	virtual State& eval(const State& cSignal, State& cOutput) const = 0;
	virtual State& evalDerivative(const State& cSignal, State& cOutput) const = 0;
};

class fLinear : public ActivationFcn {
public:
	State& eval(const State& cSignal, State& cOutput) const;
	State& evalDerivative(const State& cSignal, State& cOutput) const;
};

class fRBF : public ActivationFcn {
public:
	State& eval(const State& cSignal, State& cOutput) const;
	State& evalDerivative(const State& cSignal, State& cOutput) const;
};

class fTanh : public ActivationFcn {
public:
	State& eval(const State& cSignal, State& cOutput) const;
	State& evalDerivative(const State& cSignal, State& cOutput) const;
};

#endif
