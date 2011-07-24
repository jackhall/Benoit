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
	virtual double eval(const double dSignal) const = 0;
	virtual double evalDerivative(const double dSignal) const = 0;
};

class fLinear : public ActivationFcn {
public:
	virtual double eval(const double dSignal) const;
	virtual double evalDerivative(const double dSignal) const;
};

class fRBF : public ActivationFcn {
public:
	virtual double eval(const double dSignal) const;
	virtual double evalDerivative(const double dSignal) const;
};

class fTanh : public ActivationFcn {
public:
	virtual double eval(const double dSignal) const;
	virtual double evalDerivative(const double dSignal) const;
};

#endif
