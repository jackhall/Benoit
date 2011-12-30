#ifndef BenoitIndex_h
#define BenoitIndex_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <map>
#include <iostream>
//#include <mutex>

namespace ben {

	template<typename W, typename S> class Node;

	template<typename W, typename S> 
	class Index {
	
	private:
		//std::mutex readLock, writeLock;
		std::map< unsigned int, Node<W,S>* > IDMap;
		
	public:
		Index()=default;
		~Index();
		Index(const Index& rhs) = delete; //allow copying of whole network? 
		Index& operator=(const Index& rhs) = delete; //assign whole network?
		Node<W,S>* find(const unsigned int address) const;
		bool contains(const unsigned int address) const;
		
		void add(Node<W,S>* const pNode); //only called by Node constructor?
		void remove(const unsigned int address);
		bool update(Node<W,S>* const pNode);
		
		void move_to(Index& destination, const unsigned int address);
		void swap_with(Index& other);
		void merge_into(Index& other);
			
	}; //class Index
	
} //namespace ben

#endif
