#ifndef BenoitLink_cpp
#define BenoitLink_cpp

namespace Benoit {
	//=================== CTOR, DTOR ==========================
	template<typename T, typename W, typename S, typename E>
	Link::Link() 
		: index(NULL), origin(0), target(0), weight(0.0) {
	}
	
	template<typename T, typename W, typename S, typename E>
	Link::Link(const Index<T,W,S,E>* pIndex,
		   const unsigned int nOrigin, 
		   const unsigned int nTarget, 
		   const W wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight) {
	}
	
	template<typename T, typename W, typename S, typename E>
	Link::Link(const Link&& rhs) 
		: index(rhs.index), origin(rhs.origin), target(rhs.target), weight(rhs.weight)
		  forward(rhs.forward), backward(rhs.forward) {
		//move semantics for mutex?
		//replace old pointer in Index and at Nodes
	}
	
	template<typename T, typename W, typename S, typename E>
	Link::~Link() {
		//only called when Index deletes the last shared_ptr
		//cleanup at Nodes is handled through Index
	}
	
	//=================== METHODS ===========================
	template<typename T, typename W, typename S, typename E>
	Link& Link::operator=(const Link&& rhs) {
		//move semantics for mutex?
		//replace old pointer in Index and at Nodes
	}
	
	template<typename T, typename W, typename S, typename E>
	S Link::pull_fore() {
		S temp;
		temp = forward.front();
		forward.pop_front();
		return temp;
	}
	
	template<typename T, typename W, typename S, typename E>
	E Link::pull_back() {
		E temp;
		temp = backward.front();
		backward.pop_front();
		return temp;
	}
	
} //namespace Benoit

#endif
