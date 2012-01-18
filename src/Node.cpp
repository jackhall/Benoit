#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=============== CTOR, DTOR ===================
	template<typename W, typename S>
	Node<W,S>::Node() 
		: ID( getNewID() ), index( &(Node::INDEX) ) {
		index->add(this);
	} //default constructor

	template<typename W, typename S>
	Node<W,S>::Node(Index<W,S>* const pIndex) 
		: ID( getNewID() ), index(pIndex) {
		//add self to manager
		index->add(this);
	} //constructor
	
	template<typename W, typename S>
	Node<W,S>::Node(Node&& rhs) 
		: ID(rhs.ID), index(rhs.index), bias(rhs.bias),
			inputs(std::move(rhs.inputs)),
			outputs(std::move(rhs.outputs)) {
		index->update_node(this);
		update_index(index); //for Links
		rhs.index = NULL;
	} //copy constructor

	template<typename W, typename S>
	Node<W,S>& Node<W,S>::operator=(const Node& rhs) {
		if(this != &rhs) {
			//if( index == NULL ) you're screwed because the Node has a duplicate ID
			if( index != rhs.index ) index->move_to(*rhs.index, ID); 
			else clear();
			
			bias = rhs.bias;
		
			//create new copies of all Links
			copy_inputs(rhs);
			copy_outputs(rhs);
		}
	} //assignment operator

	template<typename W, typename S>
	Node<W,S>::~Node() {
		//delete all Links (input and output)
		clear();
		//remove self from manager
		if(index != NULL) index->remove(ID); //index is only NULL if Node::INDEX has been destroyed
	} //destructor
	
	//==================== METHODS =====================
	template<typename W, typename S>
	bool Node<W,S>::copy_inputs(const Node& other) {
		if(index == other.index) {
			auto it = other.inputs.begin();
			auto ite = other.inputs.end();
			while(it != ite) {
				add_input(it->origin, it->weight);
				++it;
			}
			return true;
		} else return false;
		
	}
	
	template<typename W, typename S>
	bool Node<W,S>::copy_outputs(const Node& other) {
		if(index == other.index) {
			unsigned int address;
			auto it = other.outputs.begin();
			auto ite = other.outputs.end();
			while(it != ite) {
				address = (*it)->target;
				index->find(address)->add_input(ID, (*it)->weight);
				++it;
			}
			return true;
		} else return false;
	}
	
	template<typename W, typename S>
	void Node<W,S>::update_output(const Link<W,S>* const oldLink, Link<W,S>* const newLink) {
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
	
	template<typename W, typename S>
	void Node<W,S>::add_input(const unsigned int origin, const W& weight) {
		//Link will create its own pointer at the origin Node
		inputs.push_back( Link<W,S>(index, origin, ID, weight) );
	} //add_input
	
	template<typename W, typename S>
	void Node<W,S>::remove_input(const unsigned int origin) {
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
	} //remove_input
	
	template<typename W, typename S>
	void Node<W,S>::add_output(Link<W,S>* const pLink) {
		//private; for adding output Link pointer
		outputs.push_back(pLink);
	} //add_output
	
	template<typename W, typename S>
	void Node<W,S>::remove_output(Link<W,S>* const pLink) {
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
	} //remove_output
	
	template<typename W, typename S>
	void Node<W,S>::clear() {
		clear_outputs();
		clear_inputs();
		
	} //clear
	
	template<typename W, typename S>
	void Node<W,S>::clear_inputs() {
		inputs.clear();
	}
	
	template<typename W, typename S>
	void Node<W,S>::clear_outputs() {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			index->find( (*it)->target )->remove_input(ID);
			++it;
		}
	}
	
	template<typename W, typename S>
	bool Node<W,S>::contains_input(const unsigned int nOrigin) const {
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == nOrigin) return true;
			++it;
		}
		return false;
	}
	
	template<typename W, typename S>
	bool Node<W,S>::contains_output(const unsigned int nTarget) const {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if((*it)->target == nTarget) return true;
			++it;
		}
		return false;
	}
	
	template<typename W, typename S>
	bool Node<W,S>::update_index(Index<W,S>* const pIndex) {
		if(pIndex->contains(ID)) {
			index = pIndex;
			
			//update all Link::index pointers
			auto it = inputs.begin();
			auto ite = inputs.end();
			while(it != ite) {
				it->update_index(pIndex);
				++it;
			}
			return true;
		} else return false;
	} //update_index
	
	//================== port methods ======================
	//---------- constructor -----------------
	template<typename W, typename S>
	Node<W,S>::input_port::input_port(const typename std::list< Link<W,S> >::iterator iLink) 
		: current(iLink) {}
	
	template<typename W, typename S>
	Node<W,S>::output_port::output_port(const typename std::vector< Link<W,S>* >::iterator iLink) 
		: current(iLink) {}
	
	//---------- increment/decrement -------------
	//input_port
	template<typename W, typename S>
	typename Node<W,S>::input_port& Node<W,S>::input_port::operator++() {
		++current; 
		return *this;
	}

	template<typename W, typename S>
	typename Node<W,S>::input_port  Node<W,S>::input_port::operator++(int) {
		auto temp = *this;
		++current; 
		return temp;
	}	
	
	template<typename W, typename S>
	typename Node<W,S>::input_port& Node<W,S>::input_port::operator--() {
		--current; 
		return *this;
	}
	
	template<typename W, typename S>
	typename Node<W,S>::input_port  Node<W,S>::input_port::operator--(int) {
		auto temp = *this;
		--current; 
		return temp;
	}
	
	//output_port
	template<typename W, typename S>
	typename Node<W,S>::output_port& Node<W,S>::output_port::operator++() {
		++current; 
		return *this;
	}
	
	template<typename W, typename S>
	typename Node<W,S>::output_port  Node<W,S>::output_port::operator++(int) {
		auto temp = *this;
		++current; 
		return temp;
	}

	template<typename W, typename S>
	typename Node<W,S>::output_port& Node<W,S>::output_port::operator--() {
		--current; 
		return *this;
	}
	
	template<typename W, typename S>
	typename Node<W,S>::output_port  Node<W,S>::output_port::operator--(int) {
		auto temp = *this;
		--current; 
		return temp;
	}
	
	//------------- streaming operators --------------------
	template<typename W, typename S>
	typename Node<W,S>::input_port& Node<W,S>::input_port::operator>>(S& signal) {
		//pulls signal forward out of buffer, leaving the value in the buffer but passing over it
		signal = current->pull();
		return *this;
	}
	
	template<typename W, typename S>
	typename Node<W,S>::output_port& Node<W,S>::output_port::operator<<(const S& signal) {
		//pushes signal forward into buffer, overwriting whatever was in that space
		(*current)->push(signal); 
		return *this;
	}
} //namespace ben

#endif
