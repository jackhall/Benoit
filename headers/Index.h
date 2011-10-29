#ifndef BenoitIndex_h
#define BenoitIndex_h

#include <map>
#include <memory>

namespace Benoit {

	template<typename T, typename S, typename E> 
	class Node;

	template<typename T, typename S, typename E> 
	class Index {
	using namespace std;
	
	private:
		map< unsigned int, Node<T,S,E>* > mmIDMap;
		
	public:
		Index()=default;
		~Index()=default;
		Node<T,S,E>* find(const unsigned int nAddress) //access a Node via its integer ID
		inline bool contains(const unsigned int nAddress)
			{ return mmIDMap.find(nAddress) != mmIDMap.end(); }
		unsigned int insert(const T tBias);
		void move(Index& cDestination, const unsigned int nNode);
		inline void erase(const unsigned int nNode) { mmIDMap.erase(nNode); } //check ref_count
		inline void swap(Index& cOther) { mmIDMap.swap(cOther.mmIDMap); }
		inline void clear() { mmIDMap.clear(); } //check ref_counts
			
	}; //class Index
	
} //namespace Graph

//source file needs to see Node constructors
#include "Node.h"

namespace Benoit {
	#include "Index.cpp"
}

#endif
