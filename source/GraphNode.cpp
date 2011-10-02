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
///////////// adding ////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addInput(const Node<T,S,E>* pNewIn,
					 				const unsigned int nTimeDelay) {
	mvInputs.push_back( Node<T,S,E>::Connection(pNewIn, nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addInput(const unsigned int nNewIn,
					 				const unsigned int nTimeDelay) {
	Node<T,S,E>* pNewIn = find(nNewIn);
	return addInput(pNewIn, nTimeDelay);
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const Node<T,S,E>* pNewOut,
									 const unsigned int nTimeDelay) {
	mvOutputs.push_back( Node<T,S,E>::Connection(pNewIn,nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const unsigned int nNewOut,
									 const unsigned int nTimeDelay) {
	Node<T,S,E>* pNewOut = find(nNewOut);
	return addOutput(pNewOut, nTimeDelay); 

//////////////// removing //////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	Node<T,S,E>* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn);  //pointer version next
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	Node<T,S,E>* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn);  //pointer version next
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	Node<T,S,E>* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput(pOldOut);  //pointer version next
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	Node<T,S,E>* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput( pOldOut);  //pointer version next
}

//=================== Connection methods ======================
/////////constructors, destructor
template<typename T, typename S, typename E>
Node<T,S,E>::Connection( const Node* pTarget,
						const nDelay,
						const tWeight) {						
}

template<typename T, typename S, typename E>
Node<T,S,E>::~Connection() {
}

/////////streaming operators /////////////
template<typename T, typename S, typename E>
Node<T,S,E>::Connection&  operator<<(Node<T,S,E>::Connection& out, S& sSignal) {
	out.pushSignal(sSignal);
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::Connection&  operator<<(Node<T,S,E>::Connection& out, E& eError) {
	out.pushError(eError);
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::Connection&  operator>>(Node<T,S,E>::Connection& in, S& sSignal) {
	sSignal = in.pullSignal();
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::Connection&  operator>>(Node<T,S,E>::Connection& in, E& eError) {
	eError = in.pullError();
	return *this;
}

//=================== iterator methods ========================
/////////assignment /////////
template<typename T, typename S, typename E>
Node<T,S,E>::iterator& 	Node<T,S,E>::iterator::operator=(const interator& iRhs) {
	if( this != &iRhs ) {
		mpFirst = iRhs.mpFirst;
		mpLast = iRhs.mpLast;
		mpCurrent = iRhs.mpCurrent;
		mbPastEnd = iRhs.mbPastEnd;
	}
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator& Node<T,S,E>::iterator::operator+=(const int nIndex) { //similar to operator-=
	Connection* pNew = mpCurrent+nIndex;
	if( mpLast < pNew ) {
		mpCurrent = NULL;
		mbPastEnd = true;
	} else if( mpFirst > pNew ) {
		mpCurrent = NULL;
		mbPastEnd = false;
	} else {
		mpCurrent = pNew;
		mbPastEnd = false;
	}
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator& Node<T,S,E>::iterator::operator-=(const int nIndex) { //similar to operator+=
	Connection* pNew = mpCurrent-nIndex;
	if( mpLast < pNew ) {
		mpCurrent = NULL;
		mbPastEnd = true;
	} else if( mpFirst > pNew ) {
		mpCurrent = NULL;
		mbPastEnd = false;
	} else {
		mpCurrent = pNew;
		mbPastEnd = false;
	}
	return *this;
}

///////// increment, decrement ///////////
template<typename T, typename S, typename E>
Node<T,S,E>::iterator& Node<T,S,E>::iterator::operator++() {
	if( mpCurrent == mpLast ) {
		mpCurrent=NULL;
		mbPastEnd = true;
	} else if( mpCurrent != NULL ) { 
		++mpCurrent;
	} else if( !mbPastEnd ) mpCurrent = mpFirst; 
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator& Node<T,S,E>::iterator::operator--() {
	if( mpCurrent == mpLast ) {
		mpCurrent=NULL;
		mbPastEnd = true;
	} else if( mpCurrent != NULL ) { 
		--mpCurrent;
	} else if( mbPastEnd ) mpCurrent = mpLast; 
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator Node<T,S,E>::iterator::operator++(int) {
	input_iterator iTemp(*this);
	++(*this); 
	return iTemp;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator Node<T,S,E>::iterator::operator--(int) {
	input_iterator iTemp(*this);
	--(*this); 
	return iTemp;
}

////////// streaming /////////////
template<typename T, typename S, typename E>
Node<T,S,E>::iterator&  operator<<(Node<T,S,E>::iterator& out, S& sSignal) {
	in.mpCurrent->pushSignal(sSignal);
	return out;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator&  operator<<(Node<T,S,E>::iterator& out, E& eError) {
	in.mpCurrent->pushError(eError);
	return out;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator&  operator>>(Node<T,S,E>::iterator& in, S& sSignal) {
	sSignal = in.mpCurrent->pullSignal();
	return in;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator&  operator>>(Node<T,S,E>::iterator& in, E& eSignal) {
	eError = in.mpCurrent->pullError();
	return in;
}

#endif

