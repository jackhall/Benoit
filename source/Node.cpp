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
		clear();
	}

	//=================== Connection management ==================
	//---------------- adding ---------------------
	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::add_input(	const unsigned int nNewIn,
					const T tWeight) {
		
	}

	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::add_output(	const unsigned int nNewOut,
					const T tWeight) {
		
	}

	//------------------ removing -----------------------
	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::private_remove_input(const unsigned int nOrigin) {
		auto it = inputs.find(nTarget);
		inputs.erase(it);
	}
	
	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::remove_input(const unsigned int nOrigin) {
		auto it = inputs.find(nTarget);
		pIndex->find(nOldIn)->remove_output(ID);
		inputs.erase(it);
	}

	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::remove_output(const unsigned int nTarget) {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( (*it)->target == nTarget ) {
				outputs.erase(it);
				break;
			}
			++it;
		}
	}

	template<typename T, typename W, typename S, typename E>
	void Node<T,W,S,E>::clear() {
		//delete all outputs from other nodes (where they are inputs)
		auto ito = outputs.begin();
		auto itoe = outputs.end();
		while(ito != itoe) {
			find( (*it)->get_target() )->private_remove_input(ID);
			++ito;
		}
		outputs.clear();
		
		//remove all inputs from other nodes (where they are outputs)
		auto iti = inputs.begin();
		auto itie = inputs.end();
		while(iti != itie) {
			find( it->second.get_origin() )->remove_output(ID);
			++iti;
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

