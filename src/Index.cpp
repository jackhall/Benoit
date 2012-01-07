#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	template<typename W, typename S>
	Index<W,S>::Index(const Index& rhs) {
		IDMap = std::move(rhs.IDMap);
		update_all();
	}
	
	template<typename W, typename S>
	Index<W,S>::~Index() {
		//shift network to static Index (the default)!
		//if(this != Node<W,S>::INDEX) merge_into(Node<W,S>::INDEX);
		
		//in the meantime before Node has a static Index, just leave hanging Node::index pointers
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
	void Index<W,S>::add(Node<W,S>* const pNode) {
		//adds a Node pointer to this Index, removing it from its previous Index
		if( IDMap.find(pNode->ID) == IDMap.end() ) {
			IDMap.insert( std::make_pair(pNode->ID, pNode) );
			pNode->update_index(this);
		}
	} //add
	
	template<typename W, typename S>
	void Index<W,S>::remove(const unsigned int address) {
		//removes a Node from this Index
		//transfer it to static Node::INDEX or null its index pointer?
		auto it = IDMap.find(address);
		if(it != IDMap.end()) IDMap.erase(it);
	} //remove
	
	template<typename W, typename S>
	bool Index<W,S>::update_node(Node<W,S>* const pNode) {
		//returns true iff Node is already a member
		auto it = IDMap.find(pNode->ID);
		if( it != IDMap.end() ) {
			it->second = pNode;
			return true;
		} else return false;
	} //update

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
			destination.add(it->second);
			it->second->update_index(&destination);
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
				it->second->index = &other;
				other.add(it->second); //calls Node::update_index
				IDMap.erase(it++);
			}
		}
	} //merge_into

} //namespace ben

#endif