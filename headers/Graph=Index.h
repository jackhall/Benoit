#ifndef GraphIndex_h
#define GraphIndex_h

#include <map>
#include <memory>

namespace Graph {

	template<typename T, typename S, typename E> 
	class Node;

	template<typename T, typename S, typename E> 
	class Index {
	using namespace std;
	
	private:
		map< unsigned int, shared_ptr<Node<T,S,E>> > mmIDMap;
		
	public:
		Index() {}
		~Index() {}
		inline weak_ptr<Node<T,S,E>> find(const unsigned int nAddress) //access a Node via its integer ID
			{ return mmIDMap[nAddress]; }
		Index& create(const T tBias);
		Index& erase(const unsigned int nNode);
		Index& claim(const unsigned int nNode);
		Index& clean();
			
	}; //class Index
	
} //namespace Graph

//source file needs to see Node constructors
#include "Graph=Node.h"

namespace Graph {
	#include "Graph=Index.cpp"
}

#endif
