#include "Connection_base.h"
#include "Neuron_base.h"

Connection_base::Connection_base(	Neuron_base* pSource,
									Neuron_base* pTarget,
							   		const bool bTrainable)
	: mbTrainable(bTrainable),
		mpSource(pSource), 
		mpTarget(pTarget),
		mbSignalFlag(false), mbErrorFlag(false) {
	mpTarget->connectInput(this);
	mpSource->connectOutput(this);
}

Connection_base::Connection_base(	const unsigned int nSource,
									const unsigned int nTarget,
							   		const bool bTrainable)
	: mbTrainable(bTrainable),
		mpSource(dynamic_cast<Neuron_base*>(Node::find(nSource))),
		mpTarget(dynamic_cast<Neuron_base*>(Node::find(nTarget))),
		mbSignalFlag(false), mbErrorFlag(false) {
	mpTarget->connectInput(this);
	mpSource->connectOutput(this);
}

Connection_base::~Connection_base() {
	mpTarget->disconnectInput(this);
	mpSource->disconnectOutput(this);
}

ostream& Connection_base::print(ostream& out) const {	
	return out;
}

