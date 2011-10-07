#ifndef GraphIndex_h
#define GraphIndex_h

#include <map>
#include <memory>

namespace Graph {

	template<typename T, typename S, typename E> 
	class Index {
		using namespace std;
	
	private:
		unsigned int mnIDCount;
		map< unsigned int, weak_ptr<Node<T,S,E>> > mmIDMap;
		
	public:
		inline unsigned int getNewID() { return mnIDCount++; }
		inline weak_ptr<Node> find(const unsigned int nAddress) //access a Node via its integer ID
			{ return mmIDMap[nAddress]; }
			
	}; //class Index
	
	#include "Graph=Index.cpp"	
	
} //namespace Graph

#endif
