#ifndef Connection_h
#define Connection_h

#include <iostream>
#include "Connection_base.h"
#include "State.h"

template<typename T> class Neuron;

template<typename T=float>
class Connection : public Connection_base {
private:
	unsigned int mnTimeDelay;

	Connection(); //hidden default constructor
	Connection(const Connection<T>& cSource);
	Connection& operator=(const Connection<T>& cSource);
	
	T add(typename State<T>::iterator iSignal) { return mtWeight+(*iSignal); }
	T multiply(typename State<T>::iterator iSignal) { return mtWeight*(*iSignal); }
	T subtract(typename State<T>::iterator iSignal) { return mtWeight-(*iSignal); }
	
	T addD(typename State<T>::iterator iError) { return *iError; }
	T multiplyD(typename State<T>::iterator iError) { return (*iError)*mtWeight; }
	T subtractD(typename State<T>::iterator iError) { return -(*iError); }

protected:
	virtual ostream& print(ostream& out) const;
	
public:
	T mtWeight;
	T (*op)(typename State<T>::iterator); 
	T (*opderiv)(typename State<T>::iterator);

	Connection( Neuron<T>* pSource,
				Neuron<T>* pTarget,
			   	const char chOperator="*",
			   	const unsigned int nTimeDelay=1,
			   	const T tWeight=1.0,
			   	const bool bTrainable=true);
	Connection( const unsigned int nSource,
				const unsigned int nTarget,
			   	const char chOperator="*",
			   	const unsigned int nTimeDelay=1,
			   	const T tWeight=1.0,
			   	const bool bTrainable=true);
	virtual ~Connection() {}
	void setOperator(const char chOperator);
};

#include "Connection.cpp"

#endif
