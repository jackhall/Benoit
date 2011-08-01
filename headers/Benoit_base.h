#ifndef Benoit_base_h
#define Benoit_base_h

#include <iostream>

using namespace std;

class Benoit_base {
protected:
	virtual ostream& print(ostream& out) const = 0;
public:
	friend ostream& operator<<(ostream& out, const Benoit_base& cBenoit_base);
};

#endif
