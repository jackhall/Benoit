#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR, DTOR ==================== 
	template<typename W, typename S, typename E>
	Node<W,S,E>::Node(const Index<W,S,E>* pIndex) 
		: ID( getNewID() ), pIndex(pIndex) {
		pIndex->add(ID,this);
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::Node(const Node& rhs)
		: ID(rhs.ID), pIndex(rhs.pIndex) {
		inputs = std::move(rhs.inputs);
		pIndex->update(ID,this);
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::~Node() {
		clear();
		pIndex->remove(ID);
	}

	//=================== Connection management ==================
	template<typename W, typename S, typename E>
	void Node<W,S,E>::update_output(const Link<W,S,E>* oldLink, const Link<W,S,E>* newLink) {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( *it == oldLink ) {
				*it = newLink;
				break;
			}
			++it;
		}
	}
	
	//---------------- adding ---------------------
	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_input(	const unsigned int origin,
					const W weight) {
		
		inputs.push_back( Link(pIndex, target, ID, weight) );
	}

	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_output(const Link<W,S,E>* pLink) {
		outputs.push_back(pLink);
	}

	//------------------ removing -----------------------
	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_input(const unsigned int origin) {
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == origin) {
				inputs.erase(it);
				break;
			}
			++it;
		}
	}

	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_output(const Link<W,S,E>* pLink) {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( *it == pLink ) {
				outputs.erase(it);
				break;
			}
			++it;
		}
	}

	template<typename W, typename S, typename E>
	void Node<W,S,E>::clear() {
		//delete all outputs from other nodes (where they are inputs)
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			pIndex->find( (*it)->get_target() )->remove_input(ID);
			++it;
		}
		
		//remove all inputs from other nodes (where they are outputs)
		inputs.clear();
	}
	
	//----------------- boolean tests -------------------
	template<typename W, typename S, typename E>
	bool Node<W,S,E>::contains_input(const unsigned int in) const {
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == in) return true;
			++it;
		}
		return false;
	}
	
	template<typename W, typename S, typename E>
	bool Node<W,S,E>::contains_output(const unsigned int out) const {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( (*it)->origin == out ) return true;
			++it;
		}
		return false;
	}
		
	//=================== iterator methods ========================
	//--------------- constructors, destructor ------------
	//input_iterator version
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator::input_iterator( list< Link<W,S,E> >::iterator iLink ) 
		: current(iLink) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator::input_iterator(const input_iterator& rhs) 
		: current(rhs.current) {	
	}
	
	template<typename W, typename S, typename E>
	input_iterator& Node<W,S,E>::input_iterator::operator=(const input_iterator& rhs) {
		current = rhs.current;
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator::~input_iterator() {
		//unlock if necessary
	}
	
	//output_iterator version
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator::output_iterator( vector< Link<W,S,E>* >::iterator iLink ) 
		: current(iLink) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator::output_iterator(const output_iterator& rhs) 
		: current(rhs.current) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator& Node<W,S,E>::output_iterator::operator=(const output_iterator& rhs) {
		current = rhs.current;
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator::~output_iterator() {
		//unlock if necessary
	}
	
	//---------------- increment, decrement ---------------
	//input_iterator version
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator Node<W,S,E>::input_iterator::operator++() {
		++current; 
		return *this;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator Node<W,S,E>::input_iterator::operator--() {
		--current; 
		return *this;
	}

	//output_iterator version
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator Node<W,S,E>::output_iterator::operator++() {
		++current; 
		return *this;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator Node<W,S,E>::output_iterator::operator--() {
		--current; 
		return *this;
	}

	//---------------- streaming ------------------
	//input_iterator
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator&  operator<<(Node<W,S,E>::input_iterator& out, 
						   E& eError) {
		out.current->push_back(eError);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator&  operator>>(Node<W,S,E>::input_iterator& in, 
						   S& sSignal) {
		sSignal = in.current->pull_fore();
		return in;
	}
	
	//output_iterator
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator&  operator<<(Node<W,S,E>::output_iterator& out, 
						    S& sSignal) {
		(*out.current)->push_fore(sSignal);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator&  operator>>(Node<W,S,E>::output_iterator& in, 
						    E& eError) {
		eError = (*in.current)->pull_back();
		return in;
	}
	
} //namespace ben

#endif

