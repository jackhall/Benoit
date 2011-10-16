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
Node<T,S,E>::Node() 
	: ID( getNewID() ) {}

template<typename T, typename S, typename E>
Node<T,S,E>::Node(const T tBias) 
	: ID( getNewID() ), mtBias(tBias) {}

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
	shared_ptr<Node> pTarget( find(nNewIn) );
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<E>);
	mvInputs.push_back( Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node> pSelf( find(ID) );
	pTarget->mvOutputs.push_back( Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const unsigned int nNewIn,
									 const T tWeight,
					 				 const unsigned int nTimeDelay) {
	using namespace std;
	shared_ptr<Node> pTarget( find(nNewIn) );
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<S>);
	mvOutputs.push_back( Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node> pSelf( find(ID) );
	pTarget->mvInputs.push_back( Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

//////////////// removing //////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	using namespace std;
	shared_ptr<Node> pSelf( find(ID) );
	shared_ptr<Node> pTarget( find(nOldIn) );
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
	shared_ptr<Node> pSelf( find(ID) );
	shared_ptr<Node> pTarget( find(nOldIn) );
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

//=================== iterator methods ========================
///////// increment, decrement ///////////
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
Node<T,S,E>::output_iterator&  operator<<(Node<T,S,E>::output_iterator& out, S& sSignal) {
	out.mpCurrent->push(sSignal);
	return out;
}

template<typename T, typename S, typename E>
Node<T,S,E>::output_iterator&  operator>>(Node<T,S,E>::output_iterator& in, E& eSignal) {
	in.mpCurrent->pull(eError);
	return in;
}

template<typename T, typename S, typename E>
Node<T,S,E>::input_iterator&  operator<<(Node<T,S,E>::input_iterator& out, E& eError) {
	out.mpCurrent->push(eError);
	return out;
}

template<typename T, typename S, typename E>
Node<T,S,E>::input_iterator&  operator>>(Node<T,S,E>::input_iterator& in, S& sSignal) {
	in.mpCurrent->pull(sSignal);
	return in;
}

#endif

