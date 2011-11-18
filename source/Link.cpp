#ifndef BenoitLink_cpp
#define BenoitLink_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR, DTOR ==========================
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const unsigned int nOrigin, 
		   	  const unsigned int nTarget, 
		   	  const W wWeight)
		: origin(nOrigin), target(nTarget), weight(wWeight) {}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const Link&& rhs) 
		: index(rhs.index), origin(rhs.origin), target(rhs.target), weight(rhs.weight)
		  forward(rhs.forward), backward(rhs.forward) {
		//move semantics for mutex?
		//replace old pointer in Index and at Nodes
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
	
} //namespace ben

#endif
