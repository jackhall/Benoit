#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	
	template<typename W, typename S> 
	class Link {
	private: 
		S buffer[2]; 
		bool mark[2]; //[read write]
		Index<W,S>* index;
		//also manages its pointer stored at the origin Node
	
	public:
		const unsigned int origin, target;
		W weight;
		
		Link() = delete;
		Link(Index<W,S>* const pIndex,
		     const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W& wWeight);
		Link(const Link& rhs); //for use by STL container
		Link& operator=(const Link& rhs) = delete; 
		~Link();
		
		void push(const S& data);
		S pull();
		
		void update_index(const Index<W,S>* pIndex);
	}; //class Link
} //namespace ben

#endif
