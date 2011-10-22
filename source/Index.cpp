#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp

template<typename T, typename S, typename E>
unsigned int Index<T,S,E>::insert(const T tBias) {
	shared_ptr< Node<T,S,E> > pTemp( new Node<T,S,E>(tBias, this) );
	mmIDMap.insert( pair< unsigned int, shared_ptr<Node<T,S,E>> >(pTemp->ID, pTemp) );
	return pTemp->ID;
}

template<typename T, typename S, typename E>
void Index<T,S,E>::move(Index<T,S,E>& cDestination, const unsigned int nNode) {
	auto it = mmIDMap.find(nNode);
	if(it != mmIDMap.end()) {
		it->disconnectAll();
		cDestination.mmIDMap.insert( pair< unsigned int, shared_ptr<Node<T,S,E>> >((*it)->ID, *it) );
		mmIDMap.erase(it);
	} 
	return;
} 

template<typename T, typename S, typename E>
weak_ptr<Node<T,S,E>> Index<T,S,E>::find(const unsigned int nAddress) {
	auto it = mmIDMap.find(nAddress);
	if(it == mmIDMap.end()) return weak_ptr<Node<T,S,E>>();
	else return *it;
}

#endif
