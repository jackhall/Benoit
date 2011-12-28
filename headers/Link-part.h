#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	
	template<typename W, typename S, typename E> 
	class Link {
	private:
		W weight; 
		S forward[2]; 
		E backward[2]; 
		bool mark[4]; //change to bitwise array later [read write read write]
		Index<W,S,E>* index;
	
	public:
		const unsigned int origin, target;
		
		Link() = delete;
		Link(Index<W,S,E>* const pIndex,
		     const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W& wWeight);
		~Link();
		
		void push_fore(const S& data);
		void push_back(const E& data);
		S pull_fore();
		E pull_back();
		
		void update_index(const Index<W,S,E>* pIndex);
		
		void set_weight(const W& newWeight) { weight = newWeight; }
		W get_weight() const { return weight; }
	}; //class Link
} //namespace ben

#endif
