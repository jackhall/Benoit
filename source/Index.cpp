#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	template<typename W, typename S>
	Index<W,S>::~Index() {
		//shift network to static index (the default)
		//if(this != Node<W,S>::INDEX) merge_into(Node<W,S>::INDEX);
	}

	template<typename W, typename S>
	Node<W,S>* Index<W,S>::find(const unsigned int nAddress) const {
		auto it = IDMap.find(nAddress);
		if(it == IDMap.end()) return NULL;
		else return it->second;
	}

	template<typename W, typename S>
	bool Index<W,S>::contains(const unsigned int address) const {
		return IDMap.end() != IDMap.find(address);
	}
	
	
	template<typename W, typename S>
	void Index<W,S>::add(Node<W,S>* const pNode) {
		if( IDMap.find(pNode->ID) == IDMap.end() ) {
			IDMap.insert( std::make_pair(pNode->ID, pNode) );
		}
	} 
	
	template<typename W, typename S>
	void Index<W,S>::remove(const unsigned int address) {
		auto it = IDMap.find(address);
		if(it != IDMap.end()) IDMap.erase(it);
	}
	
	template<typename W, typename S>
	bool Index<W,S>::update(Node<W,S>* const pNode) {
		//returns true iff Node is already a member
		auto it = IDMap.find(pNode->ID);
		if( it != IDMap.end() ) {
			it->second = pNode;
			return true;
		} else return false;
	}

	template<typename W, typename S>
	void Index<W,S>::move_to(Index<W,S>& destination, const unsigned int address) {
		//removes Node from graph and moves it to destination Index
		auto it = IDMap.find(address);
		if(it != IDMap.end()) {
			it->second->clear();
			destination.add(it->second);
			it->second->update_index(&destination);
			IDMap.erase(it);
		} 
		return;
	} 
	
	template<typename W, typename S>
	void Index<W,S>::swap_with(Index& other) {
		//uses move semantics to trade IDMaps
		auto temp = std::move(IDMap);
		IDMap = std::move(other.IDMap);
		other.IDMap = std::move(temp);
	}
	
	template<typename W, typename S>
	void Index<W,S>::merge_into(Index& other) {
		//moves all Nodes to other Index, keeping Links intact
		//current Index is left empty
		if(this != &other) {
			auto it = IDMap.begin();
			auto ite = IDMap.end();
			while(it != ite) {
				it->second->update_index(&other);
				other.add(it->second);
				IDMap.erase(it++);
			}
		}
	}

} //namespace ben

#endif
