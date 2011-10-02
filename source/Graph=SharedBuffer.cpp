#ifndef GraphSharedBuffer_cpp
#define GraphSharedBuffer_cpp

template<typename T>
SharedBuffer<T>::SharedBuffer(const unsigned int nSize) 
	: mnSize(nSize) {
	mpData = new T[nSize];
	mpCurrent = mpData;
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

#endif
