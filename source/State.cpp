#include "State.h"

State::State() 
	: mnSteps(1), mnSamples(1), mnCapacity(1), 
		mnCurrentStep(0), mnCurrentDatum(0) {
	mpData = new double[1];
	mpData[0] = 0.0;
}

State::State(const unsigned int nSamples,
			 const unsigned int nSteps)
	: mnSteps(nSteps), mnSamples(nSamples), 
		mnCapacity(nSteps*nSamples), mnCurrentStep(0),
		mnCurrentDatum(0) {
	mpData = new double[mnCapacity];
	zero();
}

State::State(const State& cSource) 
	: mnSteps(cSource.mnSteps), mnSamples(cSource.mnSamples),
		mnCapacity(cSource.mnCapacity), mnCurrentStep(cSource.mnCurrentStep),
		mnCurrentDatum(cSource.mnCurrentDatum) {
	mpData = new double[mnCapacity];
	for(int i=cSource.getSize()-1; i>=0; i--) mpData[i] = cSource.mpData[i];
}
	
State& State::operator=(const State& cSource) {
	setSize(cSource.getSamplesN(), cSource.getStepsN());
	for(int i=cSource.getSize()-1; i>=0; i--) mpData[i] = cSource.mpData[i];
}

State::~State() {
	delete[] mpData;
}

double& State::operator()(	const unsigned int nSample, 
							const unsigned int nStepBack) { 
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw;
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

const double& State::operator()(const unsigned int nSample, 
								const unsigned int nStepBack) const {
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw;
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

State& State::operator++() {
	++mnCurrentStep;
	if(mnCurrentStep == mnSteps) mnCurrentStep = 0;
	return *this;
}

State& State::operator--() {
	--mnCurrentStep;
	if(mnCurrentStep == 0) mnCurrentStep = mnSteps-1;
	return *this;
}

State State::operator++(int) {
	State cTemp = *this;
	++(*this);
	return cTemp;
}

State State::operator--(int) {
	State cTemp = *this;
	--(*this);
	return cTemp;
}

inline State& operator>>(State& cState, const double dNew) {
	cState.mpData[cState.mnCurrentDatum++] = dNew;
	if(cState.mnCurrentDatum == cState.getSize()) cState.mnCurrentDatum = 0;
	return cState;
}

ostream& operator<<(ostream& out, const State& cState) {
	using namespace std;
	for(int s=0; s<cState.mnSamples; s++) {
		for(int t=0; t<cState.mnSteps; t++) out << cState(s,t) << "\t";
		out << endl;
	}
	return out;
}

State& State::zero() {
	for(int i=getSize()-1; i>=0; i--) mpData[i] = 0;
	return *this;
}

State& State::initializeStep(const double dBias) {
	int nEnd = (mnSamples+1)*mnCurrentStep;
	for(int i=mnSamples*mnCurrentStep; i<nEnd; i--) mpData[i] = dBias;
	return *this;
}

State& State::trim() {
	double* pTemp;
	int n = getSize();
	if(mnCapacity > n) {
		pTemp = new double[n];
		for(int i=0; i<n; i++) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = n;
	}
	return *this;
}

State& State::reserve(const unsigned int nCapacity){
	double* pTemp;
	if(nCapacity > mnCapacity) {
		pTemp = new double[nCapacity];
		for(int i=0; i<getSize(); i++) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = nCapacity;
	}
	return *this;
}

State& State::setSize(const unsigned int nSamples,
					  const unsigned int nSteps) {
	double* pTemp;
	if(nSamples*nSteps < mnCapacity) {
		pTemp = new double[nSamples*nSteps];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = nSamples*nSteps;
	}
	mnSamples = nSamples;
	mnSteps = nSteps;
	mnCurrentStep = mnCurrentDatum = 0;
	return *this;
}