#ifndef BenoitNode_cpp
#define BenoitNode_cpp

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

namespace ben {
	//=============== CTOR, DTOR ===================
	template<typename W, typename S>
	Node<W,S>::Node(const unsigned int nID) 
		: nodeID( nID ), index( &(Node::INDEX) ) {
		if( !index->add(*this) ) throw;
	} 

	template<typename W, typename S>
	Node<W,S>::Node(const W& wBias, const unsigned int nID) 
		: nodeID( nID ), index( &(Node::INDEX) ), bias(wBias) {
		if( !index->add(*this) ) throw;
	}
	
	template<typename W, typename S>
	Node<W,S>::Node(Index<W,S>& cIndex, const unsigned int nID) 
		: nodeID( nID ), index(&cIndex) {
		if( !index->add(*this) ) throw;
	}
	
	template<typename W, typename S>
	Node<W,S>::Node(Index<W,S>& cIndex, const W& wBias, const unsigned int nID) 
		: nodeID( nID ), index(&cIndex), bias(wBias) {
		if( !index->add(*this) ) throw;	
	} 
	
	template<typename W, typename S>
	Node<W,S>::Node(const Node& rhs) 
		: nodeID( get_new_ID() ), index(rhs.index), bias(rhs.bias) {
		if( !index->add(*this) ) throw;
		copy_inputs(rhs);
		copy_outputs(rhs);
	} //copy constructor
	
	template<typename W, typename S>
	Node<W,S>::Node(const Node& rhs, const unsigned int nID) 
		: nodeID( nID ), index(rhs.index), bias(rhs.bias) {
		if( !index->add(*this) ) throw;
		copy_inputs(rhs);
		copy_outputs(rhs);
	} //alternate copy constructor
	
	template<typename W, typename S>
	Node<W,S>::Node(Node&& rhs) 
		: nodeID(rhs.nodeID), index(rhs.index), bias(rhs.bias),
			inputs(std::move(rhs.inputs)),
			outputs(std::move(rhs.outputs)) {
		index->update_node(*this);
		update_index(index); //for Links
		rhs.index = NULL;
	} //move constructor

	template<typename W, typename S>
	Node<W,S>& Node<W,S>::operator=(const Node& rhs) {
		if(this != &rhs) {
			//if( index == NULL ) you're screwed because the Node has a duplicate ID
			if( index != rhs.index ) index->move_to(*rhs.index, nodeID); 
			else clear();
			
			bias = rhs.bias;
		
			//create new copies of all Links
			copy_inputs(rhs);
			copy_outputs(rhs);
		}
		return *this;
	} //assignment operator

	template<typename W, typename S>
	Node<W,S>& Node<W,S>::operator=(Node&& rhs) {
		if(this != &rhs) {
			//if( index == NULL ) you're screwed because the Node has a duplicate ID
			if( index != rhs.index ) index->move_to(*rhs.index, nodeID);
			else clear();
			
			nodeID = rhs.nodeID;
			bias = rhs.bias;
			
			//move links
			inputs = std::move( rhs.inputs );
			outputs = std::move( rhs.outputs );
			
			update_index(index); //for Links
			rhs.index = NULL;
		}
		return *this;
	} //assignment move operator

	template<typename W, typename S>
	Node<W,S>::~Node() {
		//delete all Links (input and output)
		clear();
		//remove self from manager
		if(index != NULL) index->remove(nodeID); //index is only NULL if Node::INDEX has been destroyed
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
		
	} //copy_inputs
	
	template<typename W, typename S>
	bool Node<W,S>::copy_outputs(const Node& other) {
		if(index == other.index) {
			unsigned int address;
			auto it = other.outputs.begin();
			auto ite = other.outputs.end();
			while(it != ite) {
				address = (*it)->target;
				index->find(address)->add_input(nodeID, (*it)->weight);
				++it;
			}
			return true;
		} else return false;
	} //copy_outputs
	
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
	} //update_output
	
	template<typename W, typename S>
	bool Node<W,S>::add_input(const unsigned int address, const W& weight) {
		//Link will create its own pointer at the origin Node
		if(index->contains(address))
			inputs.push_back( Link<W,S>(index, address, nodeID, weight) );
		else return false;
		return true;
	} //add_input
	
	template<typename W, typename S>
	void Node<W,S>::remove_input(const unsigned int address) {
		//destroys input Link to the specified Node
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == address) {
				inputs.erase(it);
				break;
			}
			++it;
		}
	} //remove_input
	
	template<typename W, typename S>
	bool Node<W,S>::add_output(const unsigned int address, const W& weight) {
		if(index->contains(address))
			index->find(address)->add_input(nodeID,weight);
		else return false;
		return true;
	} //add_output(unsigned int)
	
	template<typename W, typename S>
	void Node<W,S>::add_output(Link<W,S>* const pLink) {
		//private; for adding output Link pointer
		outputs.push_back(pLink);
	} //add_output(Link*)
	
	template<typename W, typename S>
	void Node<W,S>::remove_output(const unsigned int address) {
		index->find(address)->remove_input(nodeID);
	} //remove_output(unsigned int)
	
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
	} //remove_output(Link*)
	
	template<typename W, typename S>
	void Node<W,S>::clear() {
		clear_outputs();
		clear_inputs();
	} //clear
	
	template<typename W, typename S>
	void Node<W,S>::clear_inputs() {
		inputs.clear();
	} //clear_inputs
	
	template<typename W, typename S>
	void Node<W,S>::clear_outputs() {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			index->find( (*it)->target )->remove_input(nodeID);
			++it;
		}
	} //clear_outputs
	
	template<typename W, typename S>
	bool Node<W,S>::contains_input(const unsigned int nOrigin) const {
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == nOrigin) return true;
			++it;
		}
		return false;
	} //contains_input
	
	template<typename W, typename S>
	bool Node<W,S>::contains_output(const unsigned int nTarget) const {
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if((*it)->target == nTarget) return true;
			++it;
		}
		return false;
	} //contains_output
	
	template<typename W, typename S>
	bool Node<W,S>::update_index(Index<W,S>* const pIndex) {
		if(pIndex->contains(nodeID)) {
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
