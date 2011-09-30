#ifndef GraphNode_cpp
#define GraphNode_cpp
//(c) Jack Hall 2011, licensed under GNU LGPL v3

unsigned int Node::snID_COUNT = 0; //initialize ID numbers

/*
ostream& Graph::Node::print(ostream &out) const {
	//more here?
	out << "ID# " << ID << endl;
	return out;
}
*/
//=================== Node I/O management ==================
///////// adding inputs ////////
template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::addInput(const unsigned int nNewIn,
					 				const char chOperator,
					 				const unsigned int nTimeDelay) {
	//look up Node ID
	Node<T,U,W>* pNewIn = find(nNewIn);
	//call a Connection_base-derived constructor
	return addInput(pNewIn, chOperator, nTimeDelay); //pointer version next
}

template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::addInput(const unsigned int nNewIn,
					 				const char chOperator,
					 				const unsigned int nTimeDelay) {
	//look up Node ID
	Node<T,U,W>* pNewIn = find(nNewIn);
	//call a Connection_base-derived constructor
	return addInput(pNewIn, chOperator, nTimeDelay); //pointer version next
}

///////// adding outputs //////////
template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::addOutput(const unsigned int nNewOut,
									 const char chOperator,
									 const unsigned int nTimeDelay) {
	//look up Node ID
	Node<T,U,W>* pNewOut = find(nNewOut);
	//call a Connection_base-derived constructor
	return addOutput(pNewOut, chOperator, nTimeDelay); //pointer version next
}

template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::addOutput(const unsigned int nNewOut,
									 const char chOperator,
									 const unsigned int nTimeDelay) {
	//look up Node ID
	Node<T,U,W>* pNewOut = find(nNewOut);
	//call a Connection_base-derived constructor
	return addOutput(pNewOut, chOperator, nTimeDelay); //pointer version next
}

///////// removing inputs ////////////
template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	Node<T,U,W>* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn);  //pointer version next
}

template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	Node<T,U,W>* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn);  //pointer version next
}

///////// removing outputs ////////
template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	Node<T,U,W>* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput(pOldOut);  //pointer version next
}

template<typename T, typename U, typename W>
Node<T,U,W>&  Node<T,U,W>::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	Node<T,U,W>* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput(pOldOut);  //pointer version next
}

//=================== iterator methods ========================
/////////assignment /////////
template<typename T, typename U, typename W>
Node<T,U,W>::iterator& 	Node<T,U,W>::iterator::operator=(const interator& iRhs) {
	mpFirst = iRhs.mpFirst;
	mpLast = iRhs.mpLast;
	mpCurrent = iRhs.mpCurrent;
	mbPastEnd = iRhs.mbPastEnd;
	return this;
}

template<typename T, typename U, typename W>
Node<T,U,W>::iterator& Node<T,U,W>::iterator::operator+=(const interator& iRhs) {
	//finish later
	return this;
}

template<typename T, typename U, typename W>
Node<T,U,W>::iterator& Node<T,U,W>::iterator::operator-=(const interator& iRhs) {
	//finish later
	return this;
}

///////// increment, decrement ///////////
template<typename T, typename U, typename W>
Node<T,U,W>::iterator& Node<T,U,W>::iterator::operator++() {
	if(mpCurrent==mpLast) {
		mpCurrent=NULL;
		mbPastEnd = true;
	} else if(mpCurrent!=NULL) { 
		++mpCurrent;
	} else if(!mbPastEnd) mpCurrent = mpFirst; 
	return *this;
}

template<typename T, typename U, typename W>
Node<T,U,W>::iterator& Node<T,U,W>::iterator::operator--() {
	if(mpCurrent==mpLast) {
		mpCurrent=NULL;
		mbPastEnd = true;
	} else if(mpCurrent!=NULL) { 
		--mpCurrent;
	} else if(mbPastEnd) mpCurrent = mpLast; 
	return *this;
}

template<typename T, typename U, typename W>
Node<T,U,W>::iterator Node<T,U,W>::iterator::operator++(int) {
	input_iterator iTemp(*this);
	++(*this); 
	return iTemp;
}

template<typename T, typename U, typename W>
Node<T,U,W>::iterator Node<T,U,W>::iterator::operator--(int) {
	input_iterator iTemp(*this);
	--(*this); 
	return iTemp;
}

#endif

