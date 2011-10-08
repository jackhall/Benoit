#ifndef GraphNode_cpp
#define GraphNode_cpp
//(c) Jack Hall 2011, licensed under GNU LGPL v3

/*
ostream& Graph::Node::print(ostream &out) const {
	//more here?
	out << "ID# " << ID << endl;
	return out;
}
*/
//=================== Node logistics ====================
template<typename T, typename S, typename E>
Node<T,S,E>::Node() {

}

template<typename T, typename S, typename E>
Node<T,S,E>::~Node() {

}

//=================== Node I/O management ==================
///////////// adding ////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addInput(const unsigned int nNewIn,
									const T tWeight,
					 				const unsigned int nTimeDelay) {
	using namespace std;
	shared_ptr<Node> pTarget( mpIndex->find(nNewIn) );
	shared_ptr<S> pSignal(new S[nTimeDelay+1]);
	shared_ptr<E> pError(new E[nTimeDelay+1]);
	mvInputs.push_back( Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node> pSelf( mpIndex->find(ID) );
	pTarget->mvOutputs.push_back( Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const unsigned int nNewIn,
									 const T tWeight,
					 				 const unsigned int nTimeDelay) {
	using namespace std;
	shared_ptr<Node> pTarget( mpIndex->find(nNewIn) );
	shared_ptr<S> pSignal(new S[nTimeDelay+1]);
	shared_ptr<E> pError(new E[nTimeDelay+1]);
	mvOutputs.push_back( Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node> pSelf( mpIndex->find(ID) );
	pTarget->mvInputs.push_back( Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

//////////////// removing //////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	using namespace std;
	shared_ptr<Node> pSelf( mpIndex->find(ID) );
	shared_ptr<Node> pTarget( mpIndex->find(nOldIn) );
	vector<Connection>::iterator it( pTarget->mvOutputs.begin() );
	vector<Connection>::iterator ite( pTarget->mvOutputs.end() );
	while(it != ite)
		if(it->target.lock() == pSelf) {
			pTarget-mvOutputs.erase(it);
			break;
		}
	
	it = mvInputs.begin();
	ite = mvInputs.end();
	while(it != ite)
		if(it->target.lock() == pTarget) {
			mvInputs.erase(it);
			break;
		}
	return *this; 
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeOutput(const unsigned int nOldOut) {
	using namespace std;
	shared_ptr<Node> pSelf( mpIndex->find(ID) );
	shared_ptr<Node> pTarget( mpIndex->find(nOldIn) );
	vector<Connection>::iterator it( pTarget->mvInputs.begin() );
	vector<Connection>::iterator ite( pTarget->mvInputs.end() );
	while(it != ite)
		if(it->target.lock() == pSelf) {
			pTarget-mvInputs.erase(it);
			break;
		}
	
	it = mvOutputs.begin();
	ite = mvOutputs.end();
	while(it != ite)
		if(it->target.lock() == pTarget) {
			mvOutputs.erase(it);
			break;
		}
	return *this; 
}

/////////////// get iterators /////////////////
template<typename T, typename S, typename E>
Node<T,S,E>::iterator  Node<T,S,E>::inputBegin() {
	Connection* begin = &mvInputs[0];
	Connection* end = begin + mvInputs.size();
	return iterator(begin, end, begin);
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator  Node<T,S,E>::inputEnd() {
	Connection* begin = &mvInputs[0];
	Connection* end = begin + mvInputs.size();
	return ++iterator(begin, end, end-1);
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator  Node<T,S,E>::outputBegin() {
	Connection* begin = &mvOutputs[0];
	Connection* end = begin + mvOutputs.size();
	return iterator(begin, end, begin);
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator  Node<T,S,E>::outputEnd() {
	Connection* begin = &mvOutputs[0];
	Connection* end = begin + mvOutputs.size();
	return ++iterator(begin, end, end-1);
}

//=================== Connection methods ======================
template<typename T, typename S, typename E>
void Node<T,S,E>::Connection::push(S& sIn) {
	step(signalMarker);
	//check to make sure the space is empty?
	signal[signalMarker] = sIn;
	return;
}

template<typename T, typename S, typename E>
void Node<T,S,E>::Connection::push(E& eIn) {
	step(errorMarker);
	//check to make sure the space is empty?
	error[errorMarker] = eIn;
	return;
}

template<typename T, typename S, typename E>
S Node<T,S,E>::Connection::pullSignal() {
	unsigned int tempMarker = signalMarker;
	step(signalMarker);
	//clear space?
	return signal[tempMarker];
}

template<typename T, typename S, typename E>
E Node<T,S,E>::Connection::pullError() {
	unsigned int tempMarker = errorMarker;
	step(errorMarker);
	//clear space?
	return error[tempMarker];
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
	out.mpCurrent->push(sSignal);
	return out;
}

template<typename T, typename S, typename E>
Node<T,S,E>::iterator&  operator<<(Node<T,S,E>::iterator& out, E& eError) {
	out.mpCurrent->push(eError);
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

