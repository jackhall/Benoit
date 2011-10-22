#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU LGPL v3

/*
ostream& Graph::Node::print(ostream &out) const {
	//more here?
	out << "ID# " << ID << endl;
	return out;
}
*/
//=================== Node logistics ==================== NOT FINISHED!!!!!!!!
template<typename T, typename S, typename E>
Node<T,S,E>::Node(const T tBias, const Index<T,S,E>* pIndex) 
	: ID( getNewID() ), mtBias(tBias), mpIndex(pIndex) {}

template<typename T, typename S, typename E>
Node<T,S,E>::~Node() {
	disconnectAll();
	//Index called this destructor by destroying its reference to this Node
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectInput(shared_ptr<Node<T,S,E> pTarget) {
	using namespace std;
	auto it = mvInputs.begin();
	auto ite = mvInputs.end();
	while(it != ite)
		if(it->target.lock() == pTarget) {
			mvInputs.erase(it);
			break;
		}
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectOutput(shared_ptr<Node<T,S,E> pTarget) {
	using namespace std;
	auto it = mvOutputs.begin();
	auto ite = mvOutputs.end();
	while(it != ite)
		if(it->target.lock() == pTarget) {
			mvOutputs.erase(it);
			break;
		}
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectAll() {
	using namespace std;
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	
	auto it = mvOutputs.begin();
	auto ite = mvOutputs.end();
	while(it != ite) {
		it->target->disconnectInputs(pSelf);
		mvOutputs.erase(it);
	}
	
	auto it = mvInputs.begin();
	auto ite = mvInputs.end();
	while(it != ite) {
		it->target->disconnectOutputs(pSelf);
		mvInputs.erase(it);
	}
}

//=================== Connection management ==================
///////////// adding ////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addInput(const unsigned int nNewIn,
									const T tWeight,
					 				const unsigned int nTimeDelay) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pTarget( find(nNewIn) );
	if(!pTarget) throw "Node not found in current Index";
	
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<E>);
	mvInputs.push_back( Output_Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	pTarget->mvOutputs.push_back( Input_Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const unsigned int nNewIn,
									 const T tWeight,
					 				 const unsigned int nTimeDelay) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pTarget( find(nNewIn) );
	if(!pTarget) throw "Node not found in current Index";
	
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<S>);
	mvOutputs.push_back( Input_Connection(pTarget,pSignal,pError,tWeight,nTimeDelay) );
	
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	pTarget->mvInputs.push_back( Output_Connection(pSelf,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

//////////////// removing //////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	shared_ptr<Node<T,S,E>> pTarget( find(nOldIn) ); 
	
	if(!pTarget) return *this;	
	pTarget->disconnectOutput(pSelf);
	disconnectInput(pTarget);
	return *this; 
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeOutput(const unsigned int nOldOut) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	shared_ptr<Node<T,S,E>> pTarget( find(nOldIn) );
	
	if(!pTarget) return *this;
	pTarget->disconnectInput(pSelf);
	disconnectOutput(pTarget);
	return *this; 
}

//=================== iterator methods ========================
///////// increment, decrement ///////////
//input_iterator version
template<typename T, typename S, typename E>
Node<T,S,E>::input_iterator Node<T,S,E>::input_iterator::operator++(int) {
	input_iterator iTemp(*this);
	++(*this); 
	return iTemp;
}

template<typename T, typename S, typename E>
Node<T,S,E>::input_iterator Node<T,S,E>::input_iterator::operator--(int) {
	input_iterator iTemp(*this);
	--(*this); 
	return iTemp;
}

//output_iterator version
template<typename T, typename S, typename E>
Node<T,S,E>::output_iterator Node<T,S,E>::output_iterator::operator++(int) {
	output_iterator iTemp(*this);
	++(*this); 
	return iTemp;
}

template<typename T, typename S, typename E>
Node<T,S,E>::output_iterator Node<T,S,E>::output_iterator::operator--(int) {
	output_iterator iTemp(*this);
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

