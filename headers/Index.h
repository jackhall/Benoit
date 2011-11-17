#ifndef BenoitIndex_h
#define BenoitIndex_h

#include <map>
#include <memory>
#include "Node.h"

namespace Benoit {

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
	
} //namespace Graph

//source file needs to see Node constructors
#include "Node.h"

namespace Benoit {
	#include "Index.cpp"
}

#endif
