#ifndef GraphIndex_h
#define GraphIndex_h

#include <map>
#include <memory>

namespace Graph {

	template<typename T, typename S, typename E> 
	class Index {
		using namespace std;
	
	private:
		map< unsigned int, weak_ptr<Node<T,S,E>> > mmIDMap;
		
	public:
		Index() {}
		~Index() {}
		inline weak_ptr<Node> find(const unsigned int nAddress) //access a Node via its integer ID
			{ return mmIDMap[nAddress]; }
		Index& create(const T tBias);
		Index& erase(const unsigned int nNode);
		Index& move(const unsigned int nNode);
			
	}; //class Index
	
	#include "Graph=Index.cpp"	
	
} //namespace Graph

#endif
