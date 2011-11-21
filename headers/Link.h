#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <mutex>

namespace ben {
	
	template<typename W, typename S, typename E> 
	class Link {
	
	private:
		//============ FIELDS =============
		std::mutex mlock; //locked by Node::iterators
		W weight; 
		S forward[2]; 
		E backward[2]; 
		bool mark[4]; //change to bitwise array later [read write read write]
		Index<W,S,E>* index;
		
	public:
		const unsigned int origin, target;
		
		//============ CTOR, DTOR =============
		Link() = delete;
		Link(const Index<W,S,E>* pIndex,
		     const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W& wWeight);
		Link(const Link& rhs); //need move semantics for mutex?
		~Link();
		
		//============ METHODS ================
		Link& operator=(const Link& rhs) = delete; //can't overwrite origin & target
		
		void push_fore(const S& data);
		void push_back(const E& data);
		S pull_fore();
		E pull_back();
		
		void set_weight(const W& newWeight) { weight = newWeight; }
		W get_weight() const { return weight; }
		
		void lock() 	{ mlock.lock(); }
		bool try_lock() { return mlock.try_lock(); }
		void unlock() 	{ mlock.unlock(); }
		
	}; //class Link
} //namespace ben

#endif
