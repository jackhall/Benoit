#ifndef State_h
#define State_h

#include <iostream>

using namespace std;

class State { 
private:
	double* mpData;
	unsigned int mnSteps, mnSamples, mnCapacity, mnCurrentStep, mnCurrentDatum;

public:
	State();
	State(	const unsigned int nSamples,
		const unsigned int nSteps = 1);
	State(const State& cSource);
	State& operator=(const State& cSource);
	~State();
	
	inline double& operator()(	const unsigned int nSample, 
								const unsigned int nStepBack = 0);
	inline const double& operator()(const unsigned int nSample, 
									const unsigned int nStepBack = 0) const;
	inline double& operator[](const unsigned int nIndex)
		{ return mpData[nIndex]; }
	inline const double& operator[](const unsigned int nIndex) const 
		{ return mpData[nIndex]; }
	inline State& operator++();
	inline State& operator--();
	State operator++(int);
	State operator--(int);
	friend State& operator>>(State& cState, const double dNew);
	friend ostream& operator<<(ostream& out, const State& cState);
	
	State& zero();
	State& initializeStep(const double dBias);
	
	State& trim();
	State& reserve(const unsigned int nCapacity);
	inline unsigned int getSize() const { return mnSteps*mnSamples; }
	inline unsigned int getCapacity() const { return mnCapacity; }
	State& setSize(	const unsigned int nSamples,
					const unsigned int nSteps = 1);
	inline unsigned int getSamplesN() const { return mnSamples; }
	inline unsigned int getStepsN() const { return mnSteps; }
	
};

#endif
