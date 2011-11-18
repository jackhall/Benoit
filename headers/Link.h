#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <deque>
#include <mutex>

namespace ben {
	
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
		Link() = default;
		Link(const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W Weight);
		Link(const Link&& rhs); //will need move semantics for mutex and index
		~Link() = default;
		
		//============ METHODS ================
		Link& operator=(const Link&& rhs); //will need move semantics for mutex and index
		
		void push_fore(const S data) { forward.push_back(S); }
		void push_back(const E data) { backward.push_back(E); }
		S pull_fore();
		E pull_back();
		
		unsigned int get_origin() const { return origin; }
		unsigned int get_target() const { return target; }
		
		void set_weight(const W newWeight) { weight = newWeight; }
		W get_weight() const 		   { return weight; }
		
		void lock() 	{ mlock.lock(); }
		bool try_lock() { return mlock.try_lock(); }
		void unlock() 	{ mlock.unlock(); }
		
	}; //class Link
} //namespace ben

#endif
