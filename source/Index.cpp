#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	template<typename W, typename S,typename E>
	Index<W,S,E>::~Index() {
		if(this != Node<W,S,E>.INDEX) merge_into(Node<W,S,E>.INDEX);
	}

	template<typename W, typename S,typename E>
	Node<W,S,E>* Index<W,S,E>::find(const unsigned int nAddress) {
		auto it = IDMap.find(nAddress);
		if(it == IDMap.end()) return NULL;
		else return it->second;
	}

	template<typename W, typename S,typename E>
	bool Index<W,S,E>::contains(const unsigned int address) {
		return IDMap.end() != IDmap.find(address);
	}
	
	
	/*template<typename W, typename S,typename E>
	bool Index<W,S,E>::add(const Node<W,S,E>* pNode) {
		IDMap.insert( pair< unsigned int, <Node<W,S,E>* >(pNode->ID, pNode) );
	} */
	
	template<typename W, typename S,typename E>
	void Index<W,S,E>::remove(const unsigned int address) {
		
	}
	
	template<typename W, typename S,typename E>
	bool Index<W,S,E>::update(const Node<W,S,E>* pNode) {
		
	}

	template<typename W, typename S,typename E>
	void Index<W,S,E>::move_node(Index<W,S,E>& cDestination, const unsigned int address) {
		auto it = IDMap.find(address);
		if(it != IDMap.end()) {
			it->second->clear();
			cDestination.IDMap.insert( pair< unsigned int, Node<W,S,E>* >(it-second->ID, *it) );
			IDMap.erase(it);
		} 
		return;
	} 
	
	template<typename W, typename S,typename E>
	void Index<W,S,E>::swap(Index& other) {
		auto temp = std::move(IDMap);
		IDMap = std::move(other.IDMap);
		other.IDMap = std::move(temp);
	}
	
	template<typename W, typename S,typename E>
	void Index<W,S,E>::merge_into(Index& other) {
		
	}
	
	template<typename W, typename S,typename E>
	void Index<W,S,E>::clear() {
		auto it = IDMap.begin();
		auto ite = IDMap.end();
		while(it != ite) {
			
			++it;
		}
	}

} //namespace ben

#endif
