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

	return *this;
}

#endif
