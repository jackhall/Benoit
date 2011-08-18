#include "Connection_base.h"
#include "Neuron_base.h"	//defining this in the header would create circularity

Connection_base::Connection_base(	Neuron_base* pSource,
									Neuron_base* pTarget,
							   		const bool bTrainable)
	: mbTrainable(bTrainable),
		mpSource(pSource), 
		mpTarget(pTarget),
		mbSignalFlag(false), mbErrorFlag(false) {
	//add itself to the Nodes it's connected to
	mpTarget->connectInput(this);		//private functions of Neuron_base, these
	mpSource->connectOutput(this);		//are not called by any other class
}

Connection_base::Connection_base(	const unsigned int nSource,
									const unsigned int nTarget,
							   		const bool bTrainable)
	: mbTrainable(bTrainable),
		mpSource(dynamic_cast<Neuron_base*>(Node::find(nSource))), //find and check
		mpTarget(dynamic_cast<Neuron_base*>(Node::find(nTarget))), //Node pointers
		mbSignalFlag(false), mbErrorFlag(false) {
	//add itself to the Nodes it's connected to
	mpTarget->connectInput(this);		//private functions of Neuron_base, these
	mpSource->connectOutput(this);		//are not called by any other class
}

Connection_base::~Connection_base() {
	//delete itself from the Nodes it has been connected to
	mpTarget->disconnectInput(this);	//private functions of Neuron_base, these
	mpSource->disconnectOutput(this);	//are not called by any other class
}

ostream& Connection_base::print(ostream& out) const {	
	//Node IDs here?
	return out;
}

