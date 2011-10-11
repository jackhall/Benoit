#ifndef State_h
#define State_h

#include <memory>

template<typename T>
class State {
private:
	vector< shared_ptr<T> > mvData;
	unsigned int mnSamples, mnCurrentStep;
	
public:
	//--------------- iterator ---------------------
	class iterator {
	private:
		shared_ptr<T> mpData;
		unsigned int mnCurrentSample, mnSamples;
		
	public:
		iterator()
			: mpData(), mnCurrentSample(0), mnSamples(0) {}
		iterator(shared_ptr<T> pData,
				 const unsigned int nSamples)
			: mpData(pData), mnCurrentSample(0), mnSamples(nSamples) {}
		inline T& operator*() { return mpData[mnCurrentSample]; }
		inline T& operator[](const unsigned int nIndex) { return mpData[nIndex]; }
		iterator& operator++();
		iterator& operator--();
		iterator operator++(int);
		iterator operator--(int);
		inline bool operator==(const iterator& cOther) { return mpData==cTwo.mpData; }
		inline bool operator!=(const iterator& cOther) { return !(*this==cOther); }
		//pointer arithmetic
		//comparison > < =< =>
	}; //class iterator
	
	//------------- constructors, destructor, assignment------------
	State();
	State(	const unsigned int nSamples,
			const unsigned int nSteps=1);
	State(const State& cSource);
	State& operator=(const State& cSource);
	~State();
	
	//------------- overloaded operators --------------------
	bool operator==(const State& cOther) { return this==&cOther; }
	inline bool operator!=(const State& cOther) { return !(*this==cOther); }
	State& operator++();
	State& operator--();
	State operator++(int);
	State operator--(int);
	
	//-------------- miscellaneous methods ----------------------
	iterator begin(unsigned int nStepBack=0);
	iterator end(unsigned int nStepBack=0);
	
	inline bool empty() { return size()==0; }
	inline unsigned int size() const { return mnSteps*mnSamples; }
	inline unsigned int capacity() const { return mnCapacity; }
	State& resize(	const unsigned int nSamples,
					const unsigned int nSteps = 1);
	inline unsigned int samples() const { return mnSamples; }
	inline unsigned int steps() const { return mvData.size(); }
}; //class State

#include "State-2.cpp"

#endif
