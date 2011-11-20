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
	Node<W,S,E>::Node(const Node& cRhs)
		: ID(cRhs.ID), mtBias(cRhs.mtBias), mpIndex(cRhs.mpIndex) {
		//transfer all connections, updating info at other nodes
		
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::~Node() {
		clear();
		pIndex->remove(ID);
	}

	//=================== Connection management ==================
	//---------------- adding ---------------------
	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_input(	const unsigned int origin,
					const W weight) {
		
		auto pr = inputs.insert( std::make_pair(target, Link(target, ID, weight)) );
		pIndex->find(origin)->add_output( &(pr.first->second) )
	}

	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_output(const Link<W,S,E>* pLink) {
		outputs.push_back(pLink);
	}

	//------------------ removing -----------------------
	template<typename W, typename S, typename E>
	void Node<W,S,E>::private_remove_input(const unsigned int origin) {
		auto it = inputs.find(origin);
		inputs.erase(it);
	}
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_input(const unsigned int origin) {
		auto it = inputs.find(origin);
		pIndex->find(origin)->remove_output( &(it->second) );
		inputs.erase(it);
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
		auto ito = outputs.begin();
		auto itoe = outputs.end();
		while(ito != itoe) {
			pIndex->find( (*ito)->get_target() )->private_remove_input(ID);
			++ito;
		}
		outputs.clear();
		
		//remove all inputs from other nodes (where they are outputs)
		auto iti = inputs.begin();
		auto itie = inputs.end();
		while(iti != itie) {
			pIndex->find( iti->second.get_origin() )->remove_output(ID);
			++iti;
		}
		inputs.clear();
	}
	
	//----------------- boolean tests -------------------
	template<typename W, typename S, typename E>
	bool Node<W,S,E>::contains_input(const unsigned int in) const {
		return input.count(in) == 1;
	}
	
	template<typename W, typename S, typename E>
	bool Node<W,S,E>::contains_output(const unsigned int out) const {
		return pIndex->find(out)->contains_input(ID);
	}
	
	//================= node management ==================
	template<typename W, typename S, typename E>
	void Node<W,S,E>::set_bias(const T newBias) {
		bias = newBias;
	}
	
	template<typename W, typename S, typename E>
	T Node<W,S,E>::get_bias() const {
		return bias;
	}
		
	//=================== iterator methods ========================
	//--------------- constructors, destructor ------------
	//input_iterator version
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator::input_iterator( map< unsigned int, Link<W,S,E> >::iterator iLink ) 
		: currrent(iLink) {
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
	
	//--------------- indirection ----------------
	//input_iterator version
	template<typename W, typename S, typename E>
	Link<W,S,E>&  Node<W,S,E>::input_iterator::operator*() const {
		return current->second;
	}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>*  Node<W,S,E>::input_iterator::operator->() const {
		return &(current->second);
	}
	
	//output_iterator version
	template<typename W, typename S, typename E>
	Link<W,S,E>&  Node<W,S,E>::output_iterator::operator*() const {
		return *current;
	}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>*  Node<W,S,E>::output_iterator::operator->() const {
		return current.operator->();
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
		out.current->second->push_back(eError);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::input_iterator&  operator>>(Node<W,S,E>::input_iterator& in, 
						   S& sSignal) {
		sSignal = in.current->second->pull_fore();
		return in;
	}
	
	//output_iterator
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator&  operator<<(Node<W,S,E>::output_iterator& out, 
						    S& sSignal) {
		out.current->second->push_fore(sSignal);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::output_iterator&  operator>>(Node<W,S,E>::output_iterator& in, 
						    E& eError) {
		eError = in.current->second->pull_back();
		return in;
	}
	
} //namespace ben

#endif

