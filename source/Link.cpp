#ifndef BenoitLink_cpp
#define BenoitLink_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR ==========================
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const unsigned int nOrigin, 
		   	  const unsigned int nTarget, 
		   	  const W& wWeight)
		: origin(nOrigin), target(nTarget), weight(wWeight) {}
	
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(const Link& rhs)
		: origin(rhs.origin), target(rhs.target), weight(rhs.weight)
			mlock(), foreMark(0), backMark(0) {}
	
	//=================== METHODS ==========================
	template<typename W, typename S, typename E>
	void Link<W,S,E>::push_fore(const S& data) { 
		if(foreMark == 0) {
			forward[0] = data;
			foreMark = 1;
		} else {
			forward[1] = data;
			foreMark = 0;
		}
	}
	
	template<typename W, typename S, typename E>
	void Link<W,S,E>::push_back(const E& data) { 
		if(backMark == 0) {
			backward[0] = data;
			backMark = 1;
		} else {
			backward[1] = data;
			backMark = 0;
		}
	}
	
	template<typename W, typename S, typename E>
	S Link<W,S,E>::pull_fore() {
		if(foreMark == 0) return forward[1];
		else return forward[0];
	}
	
	template<typename W, typename S, typename E>
	E Link<W,S,E>::pull_back() {
		if(backMark == 0) return backward[1];
		else return backward[0];
	}
	
} //namespace ben

#endif
