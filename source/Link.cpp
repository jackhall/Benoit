#ifndef BenoitLink_cpp
#define BenoitLink_cpp

namespace Benoit {
	//=================== CTOR, DTOR ==========================
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link() 
		: index(NULL), origin(0), target(0), weight(0.0) {
	}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const Index<T,W,S,E>* pIndex,
		   const unsigned int nOrigin, 
		   const unsigned int nTarget, 
		   const W wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight) {
	}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const Link&& rhs) 
		: index(rhs.index), origin(rhs.origin), target(rhs.target), weight(rhs.weight)
		  forward(rhs.forward), backward(rhs.forward) {
		//move semantics for mutex?
		//replace old pointer in Index and at Nodes
	}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>::~Link() {
		//only called when Index deletes the last shared_ptr
		//cleanup at Nodes is handled through Index
	}
	
	//=================== METHODS ===========================
	template<typename W, typename S, typename E>
	Link<W,S,E>& Link<W,S,E>::operator=(const Link&& rhs) {
		//move semantics for mutex?
		//replace old pointer in Index and at Nodes
	}
	
	template<typename W, typename S, typename E>
	S Link<W,S,E>::pull_fore() {
		S temp;
		temp = forward.front();
		forward.pop_front();
		return temp;
	}
	
	template<typename W, typename S, typename E>
	E Link<W,S,E>::pull_back() {
		E temp;
		temp = backward.front();
		backward.pop_front();
		return temp;
	}
	
} //namespace Benoit

#endif
