#ifndef Connection_base_h
#define Connection_base_h

#include <iostream>
#include "Benoit_base.h"
class Node;
class Neuron_base;

class Connection_base : public Benoit_base {
private:
	Connection_base();  //hidden default constructor
	Connection_base& operator=(const Connection_base& cSource);
	Connection_base(const Connection_base& cSource);

protected:
	Neuron_base* const mpSource;
	Neuron_base* const mpTarget;
	
	virtual ostream& print(ostream& out) const; //not finished
	//friend class Neuron_base; //because friend functions cause circularity
	
public:
	bool mbTrainable, mbSignalFlag, mbErrorFlag;
	
	inline Neuron_base* getSource() { return mpSource; }
	inline Neuron_base* getTarget() { return mpTarget; }
	Connection_base(Neuron_base* pSource,
					Neuron_base* pTarget,
			   		const bool bTrainable=true);
	Connection_base(const unsigned int nSource,
					const unsigned int nTarget,
					const bool bTrainable=true);
	virtual ~Connection_base();
};

#endif

