#ifndef BenoitIndex_h
#define BenoitIndex_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <map>

namespace ben {

	template<typename T, typename W, typename S, typename E> class Node;

	template<typename T, typename W, typename S, typename E> 
	class Index {
	using namespace std;
	
	private:
		map< unsigned int, Node<T,W,S,E>* > IDMap;
		
	public:
		Index()=default;
		~Index()=default;
		Node<T,W,S,E>* find(const unsigned int address) const;
		bool contains(const unsigned int address) const;
		void move(Index& destination, const unsigned int address);
		void take(Index& origin, const unsigned int address);
		void erase(const unsigned int address);
		void swap(Index& other);
		void clear();
			
	}; //class Index
	
} //namespace ben

#endif
