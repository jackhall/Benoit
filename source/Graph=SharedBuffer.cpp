#ifndef GraphSharedBuffer_cpp
#define GraphSharedBuffer_cpp

template<typename T>
SharedBuffer<T>::SharedBuffer(const unsigned int nSize) 
	: mnSize(nSize) {
	mpData = new T[nSize];
	mpCurrent = mpData;
}

template<typename T>
SharedBuffer<T>::SharedBuffer(const unsigned int nSize,
					 		  const SharedBuffer* pOther)
	: mnSize(nSize) {
	mpData = new T[nSize];
	mpCurrent = mpData;
	if(pOther->mpData = NULL) {
		pOther->mpData = mpData;
		pOther->mpCurrent = mpCurrent;
		pOther->nSize = nSize;
		pOther->mpOther = this;
	} else {
		delete[] mpData;
		mpData = NULL;
		mpCurrent = NULL;
		throw "SharedBuffer is already paired";
	}
}

template<typename T>
SharedBuffer<T>::SharedBuffer(	const SharedBuffer* pOther,
			 					const T* pData,
			 					const unsigned int nSize);
	: mpOther(pOther), mpData(pData), mnSize(nSize), mpCurrent(pData) {
	pOther->mpOther = this;
}

template<typename T>
SharedBuffer<T>::~SharedBuffer() {
	mpOther->mpData = NULL;
	mpOther->mpCurrent = NULL;
	delete[] mpData;
}

template<typename T>
void SharedBuffer<T>::stepForward() {
	if(nSize==1) return;
	else if(mpData+nSize-1==mpCurrent) mpCurrent = mpData;
	else ++mpCurrent;
}

template<typename T>
void SharedBuffer<T>::stepBack() {
	if(nSize==1) return;
	else if(mpData==mpCurrent) mpCurrent = mpData+nSize-1;
	else --mpCurrent;
}

template<typename T>
SharedBuffer<T>& operator<<(SharedBuffer<T>& out, T& tData) { //delegates to push
	push(tData);
	return *this;
}

template<typename T>
SharedBuffer<T>& operator>>(SharedBuffer<T>& in, T& tData) { //delegates to pull
	tData = pull();
	return *this;
}

#endif
