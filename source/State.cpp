#ifndef State_cpp
#define State_cpp

//(c) Jack Hall 2011, licensed under GNU LGPL v3

//default constructor
template<typename T>
State<T>::State() 
	: mnSteps(1), mnSamples(1), mnCapacity(1), 
		mnCurrentStep(0) {
	mpData = new T[1]; 		//default length of one
	mpData[0] = 0.0;
}

//constructor: takes number of samples in batch, number of time-steps to save for backpropagation-
//through-time
template<typename T>
State<T>::State(const unsigned int nSamples,
				const unsigned int nSteps)
	: mnSteps(nSteps), mnSamples(nSamples), 
		mnCapacity(nSteps*nSamples), mnCurrentStep(0) {
	mpData = new T[mnCapacity];	
	zero();
}

//standard-signature copy constructor
template<typename T>
State<T>::State(const State<T>& cSource) 
	: mnSteps(cSource.mnSteps), mnSamples(cSource.mnSamples),
		mnCapacity(cSource.mnCapacity), mnCurrentStep(cSource.mnCurrentStep) {
	//exception-safe
	T* pTemp = new T[mnCapacity]; 
	for(int i=cSource.size()-1; i>=0; i--) pTemp[i] = cSource.mpData[i];
	mpData = pTemp; 
}

//standard-signature assignment operator for State
//assume it invalidates all iterators
template<typename T>
State<T>& State<T>::operator=(const State<T>& cSource) { 
	if(this != &cSource) {
		resize(cSource.samples(), cSource.steps());
		for(int i=cSource.size()-1; i>=0; i--) mpData[i] = cSource.mpData[i];
	}
	return *this;
}

//destructor
template<typename T>
State<T>::~State() {
	delete[] mpData;
}

//parenthesis operator is used to access State as a circular array of arrays (high-level access)
//arguments: index of sample in step, number of steps backward (nStepBack=1 for previous time step)
//returns: reference of template type to the appropriate element
template<typename T>
T& State<T>::operator()(const unsigned int nSample, 
						const unsigned int nStepBack) { 
	//perform bounds-checking
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw "State(): out of bounds";
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 	//if stepping back past the beginning of the dynamic array
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

//const version of parenthesis operator, see comments in that definition
//differences: returns a const reference, guarantees no change to State
template<typename T>
const T& State<T>::operator()(	const unsigned int nSample, 
								const unsigned int nStepBack) const {
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw "State(): out of bounds";
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 	//if stepping back past the beginning of the dynamic array
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

//equality operator: compares dimensions, then compares each element until a discrepancy is found
//arguments: self-type
template<typename T>
bool State<T>::operator==(const State<T>& cTwo) {
	if(mnSamples==cTwo.mnSamples && mnSteps==cTwo.mnSteps) {
		for(unsigned int i=size()-1;i>=0;--i) {
			if(mpData[i] != cTwo.mpData[i]) return false;
		}
	} else return false;
	return true;
}

//compound addition-assignment operator: operates on each sample in the current step for each State
//returns a reference to the newly-altered State (lhs)
template<typename T>
State<T>& State<T>::operator+=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) += cNew.operator()(i);
	}
	return *this;
}

//compound subtraction-assignment operator: operates on each sample in the current step for each State
//returns a reference to the newly-altered State (lhs)
template<typename T>
State<T>& State<T>::operator-=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) -= cNew.operator()(i);
	}
	return *this;
}

//compound multiplication-assignment operator: operates on each sample in the current step for each State
//returns a reference to the newly-altered State (lhs)
template<typename T>
State<T>& State<T>::operator*=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) *= cNew.operator()(i);
	}
	return *this;
}

//compound division-assignment operator: operates on each sample in the current step for each State
//returns a reference to the newly-altered State (lhs)
template<typename T>
State<T>& State<T>::operator/=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) /= cNew.operator()(i);
	}
	return *this;
}

//postfix increment operator: increments the current time step 
//returns the State by value as it was before it was incremented
template<typename T>
State<T> State<T>::operator++(int) {
	State cTemp = *this;
	++(*this);		//delegates to prefix increment
	return cTemp;
}

//postfix decrement operator: decrements the current time step 
//returns the State by value as it was before it was decremented
template<typename T>
State<T> State<T>::operator--(int) {
	State cTemp = *this;
	--(*this);		//delegates to prefix decrement
	return cTemp;
}

//output stream operator: pushes all elements to stream in a tab-delimited matrix, 
//starting with the current time step and moving backwards (friend function)
template<typename T>
ostream& operator<<(ostream& out, const State<T>& cState) {
	using namespace std;
	for(int s=0; s<cState.mnSamples; s++) {		//through samples
		for(unsigned int t=0; t<cState.mnSteps; t++) out << cState(s,t) << "\t"; //through steps
		out << endl;
	}
	return out;
}

//arguments: number of steps back from current time step
//returns: an iterator pointing to the first element of that time step
template<typename T>
typename State<T>::iterator State<T>::begin(unsigned int nStepBack) { 
	T* pFirst = firstSample();
	T* pLast = lastSample();
	return iterator(pFirst, pLast, pFirst);
}

//arguments: number of steps back from current time step
//returns: an iterator pointing to the past-end element of that time step
template<typename T>
typename State<T>::iterator State<T>::end(unsigned int nStepBack) {
	T* pFirst = firstSample();
	T* pLast = lastSample();
	return iterator(pFirst, pLast, pLast+1);
}

//sets all elements in the State to zero
//returns: a reference to the changed State
template<typename T>
State<T>& State<T>::zero() {
	for(unsigned int i=size()-1; i>=0; i--) mpData[i] = 0;
	return *this;
}

//sets all elements in the current step to the given number
//returns: a reference to the changed State
template<typename T>
State<T>& State<T>::initializeStep(const T dBias) {
	int nEnd = (mnSamples+1)*mnCurrentStep;
	for(unsigned int i=mnSamples*mnCurrentStep; i<nEnd; i--) mpData[i] = dBias;
	return *this;
}

//reallocates all memory, reducing reserved memory to only enough to hold the StateIterator
//(trims away excess capacity)
//assume it invalidates all iterators
//returns: a reference to the changed State
template<typename T>
State<T>& State<T>::trim() {
	//exception-safe
	T* pTemp;
	unsigned int n = size();
	if(mnCapacity > n) {	//if there is excess capacity
		pTemp = new T[n];   //throws if there isn't enough memory
		for(unsigned int i=0; i<n; ++i) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = n;
	}
	return *this;
}

//reallocates all memory into a bigger block, copying all current data
//does not change the dimensions of State, only prevents future reallocation if State grows
//assume it invalidates all iterators
//arguments: target size of new flattened dynamic array
//returns: a reference to the changed State
template<typename T>
State<T>& State<T>::reserve(const unsigned int nCapacity) {
	//exception-safe
	T* pTemp;
	if(nCapacity > mnCapacity) {	//if the given capacity target is not already met
		pTemp = new T[nCapacity];	//throws if there isn't enough memory
		for(int i=0; i<size(); ++i) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = nCapacity;
	}
	return *this;
}

//changes the dimensions of State, reallocating to a bigger memory block if necessary
//leaves data uninitialized
//assume it invalidates all iterators
//arguments: number of samples in batch, number of steps to save for backpropagation-through-time
//returns: a reference to the changed State
template<typename T>
State<T>& State<T>::resize(const unsigned int nSamples,
							const unsigned int nSteps) {
	//exception-safe
	T* pTemp;
	if(nSamples != mnSamples || nSteps != mnSteps) {  //if dimensions are not already correct
		if(nSamples*nSteps < mnCapacity) {		//if there is not enough capacity reserved
			pTemp = new T[nSamples*nSteps];
			delete[] mpData;
			mpData = pTemp;
			mnCapacity = nSamples*nSteps;
		}
		mnSamples = nSamples;
		mnSteps = nSteps;
		mnCurrentStep = 0;
	}
	return *this;
}

/////////////iterator methods
////////////////////////////////////

//random-access for the iterator measured from the first element, moves the current pointer
//arguments: an index to the desired element
//returns: a reference to the appropriate element, performs bounds-checking
template<typename T>
T& State<T>::iterator::operator[](const unsigned int nIndex) {
	T* pNew = mpFirst + nIndex;
	//check bounds
	if(pNew <= mpLast) mpCurrent = pNew;
	else throw "StateIterator accessed out of bounds";
	return *mpCurrent;
}

//prefix increment operator: increments iterator, checks if the iterator's bounds status changes
//returns: a reference to the changed iterator
template<typename T>
typename State<T>::iterator& State<T>::iterator::operator++() {
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return *this;
}

//prefix decrement operator: decrements iterator, checks if iterator's bounds status changes
//returns: a reference to the changed iterator
template<typename T>
typename State<T>::iterator& State<T>::iterator::operator--() {
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return *this;
}

//postfix increment operator: delegates to prefixed version
//returns: the iterator by value before the increment
template<typename T>
typename State<T>::iterator State<T>::iterator::operator++(int) {
	iterator iTemp(*this);
	++(*this);
	return iTemp;
}

//postfix decrement operator: delegates to prefixed version
//returns: the iterator by value before the decrement
template<typename T>
typename State<T>::iterator State<T>::iterator::operator--(int) {
	iterator iTemp(*this);
	--(*this);
	return iTemp;
}

///////////const_iterator methods
////////////////////////////////////

//const version returns a const reference, otherwise identical
template<typename T>
const T& State<T>::const_iterator::operator[](const unsigned int nIndex) {
	T* pNew = mpFirst + nIndex;
	if(pNew <= mpLast) mpCurrent = pNew;
	else throw "StateIterator accessed out of bounds";
	return *mpCurrent;
}

//identical to nonconst version
template<typename T>
typename State<T>::const_iterator& State<T>::const_iterator::operator++() {
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return *this;
}

//identical to nonconst version
template<typename T>
typename State<T>::const_iterator& State<T>::const_iterator::operator--() {
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return *this;
}

//identical to nonconst version
template<typename T>
typename State<T>::const_iterator State<T>::const_iterator::operator++(int) {
	iterator iTemp(*this);
	++(*this);
	return iTemp;
}

//identical to nonconst version
template<typename T>
typename State<T>::const_iterator State<T>::const_iterator::operator--(int) {
	iterator iTemp(*this);
	--(*this);
	return iTemp;
}

#endif

