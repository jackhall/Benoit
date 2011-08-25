#ifndef Benoit_base_h
#define Benoit_base_h
/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	Benoit_base is a common parent for all classes in Benoit that represent a neural 
	structure. In the future, this may aid debugging or an external interface. As it 
	is, Benoit_base simply provides a consistent output interface. 
*/

#include <iostream>

using namespace std;

class Benoit_base {
protected:
	virtual ostream& print(ostream& out) const = 0;
	
public:
	friend ostream& operator<<(ostream& out, const Benoit_base& cBenoit_base); //just calls print
};

#endif

