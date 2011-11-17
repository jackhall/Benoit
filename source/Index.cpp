#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp

template<typename T, typename W, typename S,typename E>
unsigned int Index<T,S,E>::insert(const T tBias) {
	Node<T,S,E>* pTemp( new Node<T,S,E>(tBias, this) );
	mmIDMap.insert( pair< unsigned int, <Node<T,S,E>* >(pTemp->ID, pTemp) );
	return pTemp->ID;
}

template<typename T, typename W, typename S,typename E>
void Index<T,S,E>::move(Index<T,S,E>& cDestination, const unsigned int nNode) {
	auto it = mmIDMap.find(nNode);
	if(it != mmIDMap.end()) {
		it->disconnectAll();
		cDestination.mmIDMap.insert( pair< unsigned int, Node<T,S,E>* >((*it)->ID, *it) );
		mmIDMap.erase(it);
	} 
	return;
} 

template<typename T, typename W, typename S,typename E>
Node<T,S,E>* Index<T,S,E>::find(const unsigned int nAddress) {
	auto it = mmIDMap.find(nAddress);
	if(it == mmIDMap.end()) return null_ptr;
	else return *it;
}

#endif
