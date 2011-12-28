#ifndef BenoitLink_cpp
#define BenoitLink_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR ==========================
	template<typename W, typename S, typename E>
	Link<W,S,E>::Link(Index<W,S,E>* const pIndex,
			  const unsigned int nOrigin, 
		   	  const unsigned int nTarget, 
		   	  const W& wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight) {
		//add pointer to origin Node
		index->find(origin)->add_output(this);
		mark[0] = false;
		mark[1] = false;
		mark[2] = false;
		mark[3] = false;
	} //constructor
	
	//===================== DTOR ========================
	template<typename W, typename S, typename E>
	Link<W,S,E>::~Link() {
		//remove pointer at origin Node
		if(index != NULL) index->find(origin)->remove_output(this);
	} //destructor
	
	//=================== METHODS ==========================
	template<typename W, typename S, typename E>
	void Link<W,S,E>::push_fore(const S& data) { 
		if(mark[1]) {
			forward[0] = data;
			mark[1] = false;
		} else {
			forward[1] = data;
			mark[1] = true;
		}
	} //push_fore
	
	template<typename W, typename S, typename E>
	void Link<W,S,E>::push_back(const E& data) { 
		if(mark[3]) {
			backward[0] = data;
			mark[3] = false;
		} else {
			backward[1] = data;
			mark[3] = true;
		}
	} //push_back
	
	//add readiness checks later
	template<typename W, typename S, typename E>
	S Link<W,S,E>::pull_fore() {
		if(mark[0]) {
			mark[0] = false;
			return forward[0];
		} else {
			mark[0] = true;
			return forward[1];
		}
	} //pull_fore
	
	template<typename W, typename S, typename E>
	E Link<W,S,E>::pull_back() {
		if(mark[2]) {
			mark[2] = false;
			return backward[0];
		} else {
			mark[2] = true;
			return backward[1];
		}
	} //pull_back
	
	template<typename W, typename S, typename E>
	void Link<W,S,E>::update_index(const Index<W,S,E>* pIndex) {
		index = pIndex;
	} //update_index
	
} //namespace ben

#endif
