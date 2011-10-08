#ifndef GraphIndex_cpp
#define GraphIndex_cpp

template<typename T, typename S, typename E>
Index<T,S,E>&  Index<T,S,E>::create(const T tBias) {
	
	return *this;
}

template<typename T, typename S, typename E>
Index<T,S,E>&  Index<T,S,E>::erase(const unsigned int nNode) {

	return *this;
}

template<typename T, typename S, typename E>
Index<T,S,E>&  Index<T,S,E>::move(const unsigned int nNode) {

	return *this;
}

template<typename T, typename S, typename E>
Index<T,S,E>&  Index<T,S,E>::clean() {
	map< unsigned int, weak_ptr<Node<T,S,E>> >::iterator it = mmIDMap.begin();
	map< unsigned int, weak_ptr<Node<T,S,E>> >::iterator ite = mmIDMap.end();
	while(it != ite) 
		if( it->second.expired() )
			mmIDMap.erase(it);
	return *this;
}

#endif
