#ifndef State_cpp
#define State_cpp

template<typename T>
State<T>::State() 
	: mnSteps(1), mnSamples(1), mnCapacity(1), 
		mnCurrentStep(0) {
	mpData = new T[1];
	mpData[0] = 0.0;
}

template<typename T>
State<T>::State(const unsigned int nSamples,
				const unsigned int nSteps)
	: mnSteps(nSteps), mnSamples(nSamples), 
		mnCapacity(nSteps*nSamples), mnCurrentStep(0) {
	mpData = new T[mnCapacity];
	zero();
}

template<typename T>
State<T>::State(const State<T>& cSource) 
	: mnSteps(cSource.mnSteps), mnSamples(cSource.mnSamples),
		mnCapacity(cSource.mnCapacity), mnCurrentStep(cSource.mnCurrentStep) {
	mpData = new T[mnCapacity];
	for(int i=cSource.size()-1; i>=0; i--) mpData[i] = cSource.mpData[i];
}

template<typename T>
State<T>& State<T>::operator=(const State<T>& cSource) { 
	if(this != &cSource) {
		setSize(cSource.samples(), cSource.steps());
		for(int i=cSource.size()-1; i>=0; i--) mpData[i] = cSource.mpData[i];
	}
	return *this;
}

template<typename T>
State<T>::~State() {
	delete[] mpData;
}

template<typename T>
T& State<T>::operator()(const unsigned int nSample, 
						const unsigned int nStepBack) { 
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw "State(): out of bounds";
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

template<typename T>
const T& State<T>::operator()(	const unsigned int nSample, 
								const unsigned int nStepBack) const {
	if(nStepBack >= mnSteps || nSample >= mnSamples) throw "State(): out of bounds";
	unsigned int nIndex = nSample;
	if(mnCurrentStep - nStepBack < 0) 
		nIndex += (mnCurrentStep-nStepBack+mnSteps)*mnSamples;
	else nIndex += (mnCurrentStep-nStepBack)*mnSamples;
	return mpData[nIndex]; 
}

template<typename T>
bool State<T>::operator==(const State<T>& cTwo) {
	if(mnSamples==cTwo.mnSamples && mnSteps==cTwo.mnSteps) {
		for(unsigned int i=size()-1;i>=0;--i) {
			if(mpData[i] != cTwo.mpData[i]) return false;
		}
	} else return false;
	return true;
}

template<typename T>
State<T>& State<T>::operator+=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) += cNew.operator()(i);
	}
	return *this;
}

template<typename T>
State<T>& State<T>::operator-=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) -= cNew.operator()(i);
	}
	return *this;
}

template<typename T>
State<T>& State<T>::operator*=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) *= cNew.operator()(i);
	}
	return *this;
}

template<typename T>
State<T>& State<T>::operator/=(const State<T>& cNew) {
	for(unsigned int i=mnSamples-1;i>=0;--i) {
		this->operator()(i) /= cNew.operator()(i);
	}
	return *this;
}

template<typename T>
State<T> State<T>::operator++(int) {
	State cTemp = *this;
	++(*this);
	return cTemp;
}

template<typename T>
State<T> State<T>::operator--(int) {
	State cTemp = *this;
	--(*this);
	return cTemp;
}

template<typename T>
ostream& operator<<(ostream& out, const State<T>& cState) {
	using namespace std;
	for(int s=0; s<cState.mnSamples; s++) {
		for(unsigned int t=0; t<cState.mnSteps; t++) out << cState(s,t) << "\t";
		out << endl;
	}
	return out;
}

template<typename T>
typename State<T>::iterator State<T>::begin(unsigned int nStepBack) { 
	T* pFirst = firstSample();
	T* pLast = lastSample();
	return iterator(pFirst, pLast, pFirst);
}

template<typename T>
typename State<T>::iterator State<T>::end(unsigned int nStepBack) {
	T* pFirst = firstSample();
	T* pLast = lastSample();
	return iterator(pFirst, pLast, pLast+1);
}
	
template<typename T>
State<T>& State<T>::zero() {
	for(unsigned int i=size()-1; i>=0; i--) mpData[i] = 0;
	return *this;
}

template<typename T>
State<T>& State<T>::initializeStep(const T dBias) {
	int nEnd = (mnSamples+1)*mnCurrentStep;
	for(unsigned int i=mnSamples*mnCurrentStep; i<nEnd; i--) mpData[i] = dBias;
	return *this;
}

template<typename T>
State<T>& State<T>::trim() {
	T* pTemp;
	unsigned int n = size();
	if(mnCapacity > n) {
		pTemp = new T[n];
		for(unsigned int i=0; i<n; ++i) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = n;
	}
	return *this;
}

template<typename T>
State<T>& State<T>::reserve(const unsigned int nCapacity){
	T* pTemp;
	if(nCapacity > mnCapacity) {
		pTemp = new T[nCapacity];
		for(int i=0; i<size(); ++i) pTemp[i] = mpData[i];
		delete[] mpData;
		mpData = pTemp;
		mnCapacity = nCapacity;
	}
	return *this;
}

template<typename T>
State<T>& State<T>::resize(const unsigned int nSamples,
							const unsigned int nSteps) {
	T* pTemp;
	if(nSamples != mnSamples || nSteps != mnSteps) {
		if(nSamples*nSteps < mnCapacity) {
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

template<typename T>
T& State<T>::iterator::operator[](const unsigned int nIndex) {
	T* pNew = mpFirst + nIndex;
	if(pNew <= mpLast) mpCurrent = pNew;
	else throw "StateIterator accessed out of bounds";
	return *mpCurrent;
}

template<typename T>
typename State<T>::iterator& State<T>::iterator::operator++() {
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return *this;
}

template<typename T>
typename State<T>::iterator& State<T>::iterator::operator--() {
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return *this;
}

template<typename T>
typename State<T>::iterator State<T>::iterator::operator++(int) {
	iterator iTemp(*this);
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return iTemp;
}

template<typename T>
typename State<T>::iterator State<T>::iterator::operator--(int) {
	iterator iTemp(*this);
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return iTemp;
}

template<typename T>
const T& State<T>::const_iterator::operator[](const unsigned int nIndex) {
	T* pNew = mpFirst + nIndex;
	if(pNew <= mpLast) mpCurrent = pNew;
	else throw "StateIterator accessed out of bounds";
	return *mpCurrent;
}

template<typename T>
typename State<T>::const_iterator& State<T>::const_iterator::operator++() {
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return *this;
}

template<typename T>
typename State<T>::const_iterator& State<T>::const_iterator::operator--() {
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return *this;
}

template<typename T>
typename State<T>::const_iterator State<T>::const_iterator::operator++(int) {
	iterator iTemp(*this);
	if(mpCurrent == mpLast) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent<mpLast && mpCurrent==(mpFirst-1)) 
		mbOutOfBounds = false;
	++mpCurrent;
	return iTemp;
}

template<typename T>
typename State<T>::const_iterator State<T>::const_iterator::operator--(int) {
	iterator iTemp(*this);
	if(mpCurrent == mpFirst) mbOutOfBounds = true;
	else if(mbOutOfBounds && mpCurrent>mpFirst && mpCurrent==(mpLast+1))
		mbOutOfBounds = false;
	--mpCurrent;
	return iTemp;
}

#endif

