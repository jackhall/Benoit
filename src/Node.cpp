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
		rhs.index = NULL;
	} //copy constructor

	template<typename W, typename S>
	Node<W,S>& Node<W,S>::operator=(const Node& rhs) {
		if(this != &rhs) {
			index->remove(ID);
			index = rhs.index;
			index->add(this);
			
			bias = rhs.bias;
		
			//create new copies of all input Links
			clear();
			auto it = rhs.inputs.begin();
			auto ite = rhs.inputs.end();
			while(it != ite) {
				add_input(it->origin, it->weight);
				++it;
			}
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
		//delete all outputs from other nodes (where they are inputs)
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			index->find( (*it)->target )->remove_input(ID);
			++it;
		}
		
		//remove all inputs from other nodes (where they are outputs)
		inputs.clear();
	} //clear
	
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
			if((*it)->origin == nTarget) return true;
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
	typename Node<W,S>::input_port& operator>>(typename Node<W,S>::input_port& in, S& signal) {
		//pulls signal forward out of buffer, leaving the value in the buffer but passing over it
		signal = in.current->pull();
		return in;
	}
	
	template<typename W, typename S>
	typename Node<W,S>::output_port& operator<<(typename Node<W,S>::output_port& out, S& signal) {
		//pushes signal forward into buffer, overwriting whatever was in that space
		(*out.current)->push(signal); //this syntax is more efficient than out->current->push
		return out;
	}
} //namespace ben

#endif
