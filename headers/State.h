#ifndef State_h
#define State_h

#include <iostream>
//#include "StateIterator.h"

using namespace std;

template<typename T=float>
class State { 
private:
	T* mpData;
	unsigned int mnSteps, mnSamples, mnCapacity, mnCurrentStep;
	inline T* firstSample() { return &mpData[mnSamples*(mnCurrentStep-1)]; }
	inline T* lastSample() { return firstSample() + mnSamples-1; }

public:
	class iterator { 
	private:
		T* mpFirst;
		T* mpLast;
		T* mpCurrent;
		bool mbOutOfBounds;
	
	public:
		iterator() 
			: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbOutOfBounds(false) {}
		iterator(	const T* pFirst,
					const T* pLast,
					const T* pCurrent) 
			: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbOutOfBounds(false) {}
		inline bool inBounds() { return !mbOutOfBounds; }
		inline bool outofBounds() {return mbOutOfBounds; }
		inline T& operator*(){
			if(mbOutOfBounds) throw "Dereferenced StateIterator out of bounds";
			else return *mpCurrent; }
		T& operator[](const unsigned int nIndex);
		iterator& operator++();
		iterator& operator--();
		iterator operator++(int);
		iterator operator--(int);
		bool operator==(const iterator& cTwo) { return mpCurrent==cTwo.mpCurrent; }
		bool operator!=(const iterator& cTwo) { return mpCurrent!=cTwo.mpCurrent; }
	};
	
	class const_iterator {
	private:
		T* mpFirst;
		T* mpLast;
		T* mpCurrent;
		bool mbOutOfBounds;
	
	public:
		const_iterator() 
			: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbOutOfBounds(false) {}
		const_iterator(	const T* pFirst,
						const T* pLast,
						const T* pCurrent) 
			: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbOutOfBounds(false) {}
		inline bool inBounds() { return !mbOutOfBounds; }
		inline bool outofBounds() {return mbOutOfBounds; }
		inline const T& operator*(){
			if(mbOutOfBounds) throw "Dereferenced StateIterator out of bounds";
			else return *mpCurrent; }
		const T& operator[](const unsigned int nIndex);
		const_iterator& operator++();
		const_iterator& operator--();
		const_iterator operator++(int);
		const_iterator operator--(int);
		bool operator==(const const_iterator& cTwo) { return mpCurrent==cTwo.mpCurrent; }
		bool operator!=(const const_iterator& cTwo) { return mpCurrent!=cTwo.mpCurrent; }
	};
	
	State();
	State(	const unsigned int nSamples,
			const unsigned int nSteps = 1);
	State(const State& cSource);
	State& operator=(const State& cSource);
	~State();
	
	T& operator()(	const unsigned int nSample, 
					const unsigned int nStepBack = 0);
	const T& operator()(const unsigned int nSample, 
						const unsigned int nStepBack = 0) const;
	inline T& operator[](const unsigned int nIndex)
		{ return mpData[nIndex]; }
	inline const T& operator[](const unsigned int nIndex) const 
		{ return mpData[nIndex]; }
	bool operator==(const State& cTwo); //optimize this?
	inline bool operator!=(const State& cTwo) { return !(*this==cTwo); }
	State& operator+=(const State& cNew);
	State& operator-=(const State& cNew);
	State& operator*=(const State& cNew);
	State& operator/=(const State& cNew);
	inline State& operator++()
		{	++mnCurrentStep;
			if(mnCurrentStep == mnSteps) mnCurrentStep = 0;
			return *this; }
	inline State& operator--()
		{	if(mnCurrentStep == 0) mnCurrentStep = mnSteps;
			--mnCurrentStep;
			return *this; }
	State operator++(int);
	State operator--(int);
	
	template<typename U>
	friend ostream& operator<<(ostream& out, const State<U>& cState);
	
	iterator begin(unsigned int nStepBack=0);
	iterator end(unsigned int nStepBack=0);
	inline bool empty() { return size()==0; }
	State& zero();
	State& initializeStep(const T tBias);
	State& trim();
	State& reserve(const unsigned int nCapacity);
	inline unsigned int size() const { return mnSteps*mnSamples; }
	inline unsigned int capacity() const { return mnCapacity; }
	State& resize(	const unsigned int nSamples,
					const unsigned int nSteps = 1);
	inline unsigned int samples() const { return mnSamples; }
	inline unsigned int steps() const { return mnSteps; }
};

#include "State.cpp"

#endif

