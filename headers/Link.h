#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	
	template<typename W, typename S> 
	class Link {
	private:
		W weight; 
		S buffer[2]; 
		bool mark[2]; //[read write]
		Index<W,S>* index;
	
	public:
		const unsigned int origin, target;
		
		Link() = delete;
		Link(Index<W,S>* const pIndex,
		     const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W& wWeight);
		~Link();
		
		void push(const S& data);
		S pull();
		
		void update_index(const Index<W,S>* pIndex);
		
		void set_weight(const W& newWeight) { weight = newWeight; }
		W get_weight() const { return weight; }
	}; //class Link
} //namespace ben

#endif
