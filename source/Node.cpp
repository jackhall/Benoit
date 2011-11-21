#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR, DTOR ==================== 
	template<typename W, typename S, typename E>
	Node<W,S,E>::Node(const Index<W,S,E>* pIndex) 
		: ID( getNewID() ), pIndex(pIndex) {
		//add self to manager
		pIndex->add(ID,this);
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::Node(const Node& rhs)
		: ID(rhs.ID), pIndex(rhs.pIndex) {
		//uses move semantics to preserve Links at original locations
		inputs = std::move(rhs.inputs);
		//inform manager of new location
		pIndex->update(ID,this);
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::~Node() {
		//delete all Links (input and output)
		clear();
		//remove self from manager
		pIndex->remove(ID);
	}
	
	//=================== other methods ===================
	template<typename W, typename S, typename E>
	void Node<W,S,E>::update_index(const Index<W,S,E>* pIndex) {
		//changes index point in self and in all member Links
		index = pIndex;
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			it->update_index(pIndex);
			++it;
		}
	}
	
	//=================== Connection management ==================
	template<typename W, typename S, typename E>
	void Node<W,S,E>::update_output(const Link<W,S,E>* oldLink, const Link<W,S,E>* newLink) {
		//updates output Link pointer
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
		//Link will create its own pointer at the origin Node
		inputs.push_back( Link(pIndex, target, ID, weight) );
	}

	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_output(const Link<W,S,E>* pLink) {
		//private; for adding output Link pointer
		outputs.push_back(pLink);
	}

	//------------------ removing -----------------------
	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_input(const unsigned int origin) {
		//destroys input Link to the specified Node
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
		//removes output Link pointer
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
	//input_port version
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port::input_port( list< Link<W,S,E> >::iterator iLink ) 
		: current(iLink) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port::input_port(const input_port& rhs) 
		: current(rhs.current) {	
	}
	
	template<typename W, typename S, typename E>
	input_port& Node<W,S,E>::input_port::operator=(const input_port& rhs) {
		current = rhs.current;
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port::~input_port() {
		//unlock if necessary
	}
	
	//output_port version
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port::output_port( vector< Link<W,S,E>* >::iterator iLink ) 
		: current(iLink) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port::output_port(const output_port& rhs) 
		: current(rhs.current) {
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port& Node<W,S,E>::output_port::operator=(const output_port& rhs) {
		current = rhs.current;
	}
	
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port::~output_port() {
		//unlock if necessary
	}
	
	//---------------- increment, decrement ---------------
	//input_port version
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port Node<W,S,E>::input_port::operator++() {
		//lock current?
		++current; 
		return *this;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port Node<W,S,E>::input_port::operator--() {
		//lock current?
		--current; 
		return *this;
	}

	//output_port version
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port Node<W,S,E>::output_port::operator++() {
		//lock current?
		++current; 
		return *this;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port Node<W,S,E>::output_port::operator--() {
		//lock current?
		--current; 
		return *this;
	}

	//---------------- streaming ------------------
	//input_port
	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port&  operator<<(Node<W,S,E>::input_port& out, 
					     E& eError) {
		//pushes error backwards into buffer
		out.current->push_back(eError);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::input_port&  operator>>(Node<W,S,E>::input_port& in, 
					     S& sSignal) {
		//pulls signal forward out of buffer and deletes it from the buffer
		sSignal = in.current->pull_fore();
		return in;
	}
	
	//output_port
	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port&  operator<<(Node<W,S,E>::output_port& out, 
					      S& sSignal) {
		//pushes signal forward into buffer
		(*out.current)->push_fore(sSignal);
		return out;
	}

	template<typename W, typename S, typename E>
	Node<W,S,E>::output_port&  operator>>(Node<W,S,E>::output_port& in, 
					      E& eError) {
		//pulls error backwards out of buffer and deletes it from the buffer
		eError = (*in.current)->pull_back();
		return in;
	}
	
} //namespace ben

#endif

