#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR, DTOR ==================== 
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::Node(const T tBias, const Index<T,W,S,E>* pIndex) 
		: ID( getNewID() ), bias(tBias), pIndex(pIndex) {}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::Node(const Node& cRhs)
		: ID(cRhs.ID), mtBias(cRhs.mtBias), mpIndex(cRhs.mpIndex) {
		//transfer all connections, updating info at other nodes
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::~Node() {
		disconnectAll();
	
	}

	//=================== Connection management ==================
	//---------------- adding ---------------------
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>&  Node<T,W,S,E>::add_input(const unsigned int nNewIn,
						 const T tWeight) {
		using namespace std;
		if(!mpIndex.contains(nNewIn)) throw "Node not found in current Index";
	
		
		inputs.insert( pair<unsigned int, Link<W,S,E>>(nNewIn, );
	
		mpIndex.find(nNewIn)->mvOutputs.push_back( Input_Connection(ID,pSignal,pError,tWeight,nTimeDelay) );
		return *this;
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>&  Node<T,W,S,E>::add_output(const unsigned int nNewOut,
						  const T tWeight) {
		using namespace std;
		if(!mpIndex.contains(nNewIn)) throw "Node not found in current Index";
	
		shared_ptr<S> pSignal(new deque<S>);
		shared_ptr<E> pError(new deque<S>);
		mvOutputs.push_back( Input_Connection(nNewOut,pSignal,pError,tWeight,nTimeDelay) );
	
		mpIndex.find(nNewOut)->mvInputs.push_back( Output_Connection(ID,pSignal,pError,tWeight,nTimeDelay) );
		return *this;
	}

	//------------------ removing -----------------------
	//have both private and public versions of remove_input?
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>&  Node<T,W,S,E>::remove_input(const unsigned int nOldIn) {
		using namespace std;
		shared_ptr<Node<T,W,S,E>> pSelf( find(ID) );
		shared_ptr<Node<T,W,S,E>> pTarget( find(nOldIn) ); 
	
		if(!mpIndex.contains(nOldIn)) return *this;
		mpIndex.find(nOldIn)->disconnectOutput(ID);
		disconnectInput(nOldIn);
		return *this; 
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>&  Node<T,W,S,E>::remove_output(const unsigned int nOldOut) {
		using namespace std;
		shared_ptr<Node<T,W,S,E>> pSelf( find(ID) );
		shared_ptr<Node<T,W,S,E>> pTarget( find(nOldIn) );
	
		if(!mpIndex.contains(nOldOut)) return *this;
		mpIndex.find(nOldOut)->disconnectInput(pSelf);
		disconnectOutput(nOldOut);
		return *this; 
	}

	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::clear() {
		//delete all outputs from other nodes (where they are inputs)
		auto ito = outputs.begin();
		auto itoe = outputs.end();
		while(ito != itoe) {
			find( (*it)->get_target() )->remove_input(ID);
		}
		outputs.clear(); //may not be necessary if remove_input deletes pointers
		
		//remove all inputs from other nodes (where they are outputs)
		auto iti = inputs.begin();
		auto itie = inputs.end();
		while(iti != itie) {
			find( it->second.get_origin() )->remove_output(ID);
		}
		inputs.clear();
	}
	
	//----------------- boolean tests -------------------
	template<typename T, typename W, typename S, typename E>
	bool Node<T,W,S,E>::contains_input(const unsigned int in) const {
		return input.count(in) == 1;
	}
	
	template<typename T, typename W, typename S, typename E>
	bool Node<T,W,S,E>::contains_output(const unsigned int out) const {
		return pIndex->find(out)->contains_input(ID);
	}
	
	//================= node management ==================
	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::set_bias(const T newBias) {
		bias = newBias;
	}
	
	template<typename T, typename W, typename S, typename E>
	T Node<T,W,S,E>::get_bias() const {
		return bias;
	}
		
	
	//=================== iterator methods ========================
	//--------------- constructors, destructor ------------
	//input_iterator version
	
	//output_iterator version
	
	//--------------- indirection ----------------
	//input_iterator version
	
	//output_iterator version
	
	//---------------- increment, decrement ---------------
	//input_iterator version
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::input_iterator Node<T,W,S,E>::input_iterator::operator++() {
		input_iterator iTemp(*this);
		++(*this); 
		return iTemp;
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::input_iterator Node<T,W,S,E>::input_iterator::operator--() {
		input_iterator iTemp(*this);
		--(*this); 
		return iTemp;
	}

	//output_iterator version
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::output_iterator Node<T,W,S,E>::output_iterator::operator++() {
		output_iterator iTemp(*this);
		++(*this); 
		return iTemp;
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::output_iterator Node<T,W,S,E>::output_iterator::operator--() {
		output_iterator iTemp(*this);
		--(*this); 
		return iTemp;
	}

	//---------------- streaming ------------------
	//input_iterator
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::input_iterator&  operator<<(Node<T,W,S,E>::input_iterator& out, 
						   E& eError) {
		out.current->second->push_back(eError);
		return out;
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::input_iterator&  operator>>(Node<T,W,S,E>::input_iterator& in, 
						   S& sSignal) {
		sSignal = in.current->second->pull_fore();
		return in;
	}
	
	//output_iterator
	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::output_iterator&  operator<<(Node<T,W,S,E>::output_iterator& out, 
						    S& sSignal) {
		out.current->second->push_fore(sSignal);
		return out;
	}

	template<typename T, typename W, typename S, typename E>
	Node<T,W,S,E>::output_iterator&  operator>>(Node<T,W,S,E>::output_iterator& in, 
						    E& eError) {
		eError = in.current->second->pull_back();
		return in;
	}
	
} //namespace ben

#endif

