#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp

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
	template<typename W, typename S>
	Index<W,S>::Index(Index&& rhs) 
		: IDMap(std::move(rhs.IDMap)) {
		update_all();
	}
	
	template<typename W, typename S>
	Index<W,S>& Index<W,S>::operator=(Index&& rhs) {
		merge_into(Node<W,S>::INDEX); //keeps old network valid
		IDMap = std::move(rhs.IDMap);
		update_all();
	}
	
	template<typename W, typename S>
	Index<W,S>::~Index() {
		//RULE: never call delete on Node::INDEX!!! 
		//this if() should not be necessary, but it is
		if(this != &Node<W,S>::INDEX) merge_into(Node<W,S>::INDEX); 
	}

	template<typename W, typename S>
	Node<W,S>* Index<W,S>::find(const unsigned int nAddress) const {
		//searches for a Node pointer in this Index by its address
		auto it = IDMap.find(nAddress); //will return a past-the-end iterator if not found
		if(it == IDMap.end()) return NULL; 
		else return it->second;
	} //find

	template<typename W, typename S>
	bool Index<W,S>::contains(const unsigned int address) const {
		//returns true if this Index contains the indicated Node
		return IDMap.end() != IDMap.find(address);
	} //contains
	
	
	template<typename W, typename S>
	bool Index<W,S>::add(Node<W,S>& node) {
		//adds a Node pointer to this Index, removing it from its previous Index
		//if statement may not be necessary since add is private
		if( !contains(node.ID) ) {
			IDMap.insert( std::make_pair(node.ID, &node) );
			node.update_index(this);
			return true;
		} else return false;
	} //add
	
	template<typename W, typename S>
	void Index<W,S>::remove(const unsigned int address) {
		//removes a Node from this Index
		//transfer it to static Node::INDEX or null its index pointer?
		auto it = IDMap.find(address);
		if(it != IDMap.end()) IDMap.erase(it);
	} //remove
	
	template<typename W, typename S>
	bool Index<W,S>::update_node(Node<W,S>& node) {
		//returns true iff Node is already a member
		auto it = IDMap.find(node.ID);
		if( it != IDMap.end() ) {
			it->second = &node;
			return true;
		} else return false;
	} //update_node

	template<typename W, typename S>
	void Index<W,S>::update_all() {
		//update Node::index pointers of all managed Nodes
		auto it = IDMap.begin();
		auto ite = IDMap.end();
		while(it != ite) {
			it->second->update_index(this);
			++it;
		} 
	} //update_all

	template<typename W, typename S>
	void Index<W,S>::move_to(Index<W,S>& destination, const unsigned int address) {
		//removes Node from graph and moves it to destination Index
		//calls: Node::clear(), Index::add(Node* const), Node::update_index(Index* const)
		auto it = IDMap.find(address); 
		if(it != IDMap.end()) {
			it->second->clear();
			destination.add( *(it->second) );
			IDMap.erase(it);
		} 
		return;
	} //move_to
	
	template<typename W, typename S>
	void Index<W,S>::swap_with(Index& other) {
		//uses move semantics to trade IDMaps
		auto temp = std::move(IDMap);
		IDMap = std::move(other.IDMap);
		other.IDMap = std::move(temp);
		
		//update all Node::index pointers in this Index
		update_all();
		
		//update all Node::index pointers in other Index
		other.update_all();
	} //swap_with
	
	template<typename W, typename S>
	void Index<W,S>::merge_into(Index& other) {
		//moves all Nodes to other Index, keeping Links intact
		//current Index is left empty
		//calls: Index::add(Node* const)
		if(this != &other) {
			auto it = IDMap.begin();
			auto ite = IDMap.end();
			while(it != ite) {
				if( !other.add( *(it->second) )) { //calls Node::update_index
					//delete node or make a copy with a new ID?
				} 
				IDMap.erase(it);
				++it;
			}
		}
	} //merge_into
	
	////////////////////////////////////////////////////
	//iterator methods
	template<typename W, typename S>
	Index<W,S>::iterator::iterator(const typename std::map<unsigned int, Node<W,S>* >::iterator iNode) 
		: current(iNode) {}
	
	template<typename W, typename S>
	typename Index<W,S>::iterator&  Index<W,S>::iterator::operator++() {
		++current;
		return *this;
	}
	
	template<typename W, typename S>
	typename Index<W,S>::iterator   Index<W,S>::iterator::operator++(int) {
		auto temp = *this;
		++current;
		return temp;
	}
	
	template<typename W, typename S>
	typename Index<W,S>::iterator&  Index<W,S>::iterator::operator--() {
		--current;
		return *this;
	}
	
	template<typename W, typename S>
	typename Index<W,S>::iterator   Index<W,S>::iterator::operator--(int) {
		auto temp = *this;
		--current;
		return temp;
	}

} //namespace ben

#endif
