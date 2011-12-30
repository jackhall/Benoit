#ifndef BenoitLink_cpp
#define BenoitLink_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=================== CTOR ==========================
	template<typename W, typename S>
	Link<W,S>::Link(Index<W,S>* const pIndex,
			const unsigned int nOrigin, 
		   	const unsigned int nTarget, 
		   	const W& wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight) {
		//add pointer to origin Node
		index->find(origin)->add_output(this);
		mark[0] = false;
		mark[1] = false;
	} //constructor
	
	template<typename W, typename S>
	Link<W,S>::Link(const Link& rhs) {
	
	} //copy constructor
	
	//===================== DTOR ========================
	template<typename W, typename S>
	Link<W,S>::~Link() {
		//remove pointer at origin Node
		if(index != NULL) index->find(origin)->remove_output(this);
	} //destructor
	
	//=================== METHODS ==========================
	template<typename W, typename S>
	void Link<W,S>::push(const S& data) { 
		if(mark[1]) {
			buffer[1] = data;
			mark[1] = false;
		} else {
			buffer[0] = data;
			mark[1] = true;
		}
	} //push
	
	//add readiness checks later
	template<typename W, typename S>
	S Link<W,S>::pull() {
		if(mark[0]) {
			mark[0] = false;
			return buffer[1];
		} else {
			mark[0] = true;
			return buffer[0];
		}
	} //pull
	
	template<typename W, typename S>
	void Link<W,S>::update_index(const Index<W,S>* pIndex) {
		index = pIndex;
	} //update_index
	
} //namespace ben

#endif
