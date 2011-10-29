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
Node<T,S,E>::Node(const Node& cRhs)
	: ID(cRhs.ID), mtBias(cRhs.mtBias), mpIndex(cRhs.mpIndex) {
	//transfer all connections, updating info at other nodes
}

template<typename T, typename S, typename E>
Node<T,S,E>::~Node() {
	disconnectAll();
	//Index called this destructor by destroying its reference to this Node
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectInput(const unsigned int nTarget) {
	using namespace std;
	auto it = mvInputs.begin();
	auto ite = mvInputs.end();
	while(it != ite)
		if(it->target == nTarget) {
			mvInputs.erase(it);
			break;
		}
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectOutput(const unsigned int nTarget) {
	using namespace std;
	auto it = mvOutputs.begin();
	auto ite = mvOutputs.end();
	while(it != ite)
		if(it->target == nTarget) {
			mvOutputs.erase(it);
			break;
		}
}

template<typename T, typename S, typename E>
void Node<T,S,E>::disconnectAll() {
	using namespace std;
	
	auto it = mvOutputs.begin();
	auto ite = mvOutputs.end();
	while(it != ite) {
		find(it->target)->disconnectInput(ID); //trusts find to locate Node
		mvOutputs.erase(it);
	}
	
	auto it = mvInputs.begin();
	auto ite = mvInputs.end();
	while(it != ite) {
		find(it->target)->disconnectOutput(ID); //trusts find to locate Node
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
	if(!mpIndex.contains(nNewIn)) throw "Node not found in current Index";
	
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<E>);
	mvInputs.push_back( Output_Connection(nNewIn,pSignal,pError,tWeight,nTimeDelay) );
	
	mpIndex.find(nNewIn)->mvOutputs.push_back( Input_Connection(ID,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::addOutput(const unsigned int nNewOut,
									 const T tWeight,
					 				 const unsigned int nTimeDelay) {
	using namespace std;
	if(!mpIndex.contains(nNewIn)) throw "Node not found in current Index";
	
	shared_ptr<S> pSignal(new deque<S>);
	shared_ptr<E> pError(new deque<S>);
	mvOutputs.push_back( Input_Connection(nNewOut,pSignal,pError,tWeight,nTimeDelay) );
	
	mpIndex.find(nNewOut)->mvInputs.push_back( Output_Connection(ID,pSignal,pError,tWeight,nTimeDelay) );
	return *this;
}

//////////////// removing //////////////////
template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeInput(const unsigned int nOldIn) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	shared_ptr<Node<T,S,E>> pTarget( find(nOldIn) ); 
	
	if(!mpIndex.contains(nOldIn)) return *this;
	mpIndex.find(nOldIn)->disconnectOutput(ID);
	disconnectInput(nOldIn);
	return *this; 
}

template<typename T, typename S, typename E>
Node<T,S,E>&  Node<T,S,E>::removeOutput(const unsigned int nOldOut) {
	using namespace std;
	shared_ptr<Node<T,S,E>> pSelf( find(ID) );
	shared_ptr<Node<T,S,E>> pTarget( find(nOldIn) );
	
	if(!mpIndex.contains(nOldOut)) return *this;
	mpIndex.find(nOldOut)->disconnectInput(pSelf);
	disconnectOutput(nOldOut);
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

