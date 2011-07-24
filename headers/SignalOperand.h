#ifndef SignalOperand_h
#define SignalOperand_h

#include <string>
#include <vector>

using namespace std;

class SignalOperand {
private:
	string mstrName;
	
protected:
	SignalOperand(string strName) : mstrName(strName) {}
	
public:
	string getName() const { return mstrName; }
	virtual double eval(const double dWeight, const vector<double> vdIn) const = 0;
	virtual double eval(const double dWeight, const double dIn) const = 0;
	virtual vector<double> evalDerivative(const double dWeight, const vector<double> vdIn) const = 0;
	virtual double evalDerivative(const double dWeight, const double dIn) const = 0;
};

using std::vector;

class OpAdd : public SignalOperand {
public:
	OpAdd() : SignalOperand("add") {}
	double eval(const double dWeight, const vector<double> vdIn) const;
	double eval(const double dWeight, const double dIn) const;
	vector<double> evalDerivative(const double dWeight, const vector<double> vdIn) const;
	double evalDerivative(const double dWeight, const double dIn) const;
};

class OpMultiply : public SignalOperand {
public:
	OpMultiply() : SignalOperand("multiply") {}
	double eval(const double dWeight, const vector<double> vdIn) const;
	double eval(const double dWeight, const double dIn) const;
	vector<double> evalDerivative(const double dWeight, const vector<double> vdIn) const;
	double evalDerivative(const double dWeight, const double dIn) const;
};

#endif
