#ifndef BenoitLink_h
#define BenoitLink_h

#include <deque>
#include <mutex>

namespace Benoit {
	
	template<typename W, typename S, typename E> 
	class Link {
	
	using namespace std;
	private:
		//============ FIELDS =============
		mutex mlock; //it is Node's responsibility to lock and unlock the link
		W weight; 
		deque<S> forward; 
		deque<E> backward; 
		unsigned int origin, target;
		
	public:
		//============ CTOR, DTOR =============
		Link();
		Link(const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W Weight);
		Link(const Link&& rhs); //will need move semantics for mutex and index
		~Link();
		
		//============ METHODS ================
		Link& operator=(const Link&& rhs); //will need move semantics for mutex and index
		
		void push_fore(const S data) { forward.push_back(S); }
		void push_back(const E data) { backward.push_back(E); }
		S pull_fore();
		E pull_back();
		
		void set_weight(const W newWeight) { weight = newWeight; }
		W get_weight() const 		   { return weight; }
		
		void lock() 	{ mlock.lock(); }
		bool try_lock() { return mlock.try_lock(); }
		void unlock() 	{ mlock.unlock(); }
		
	}; //class Link
} //namespace Benoit

#include "Link.cpp"

#endif
