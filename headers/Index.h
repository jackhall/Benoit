#ifndef BenoitIndex_h
#define BenoitIndex_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <map>
#include <iostream>
#include <mutex>

namespace ben {

	template<typename W, typename S, typename E> class Node;

	template<typename W, typename S, typename E> 
	class Index {
	
	private:
		std::mutex readLock, writeLock;
		std::map< unsigned int, Node<W,S,E>* > IDMap;
		
	public:
		Index()=default;
		~Index();
		Index(const Index& rhs) = delete;
		Index& operator=(const Index& rhs) = delete;
		Node<W,S,E>* find(const unsigned int address) const;
		bool contains(const unsigned int address) const;
		
		void add(Node<W,S,E>* const pNode); //only called by Node constructor?
		void remove(const unsigned int address);
		bool update(Node<W,S,E>* const pNode);
		
		void move_to(Index& destination, const unsigned int address);
		void swap_with(Index& other);
		void merge_into(Index& other);
			
	}; //class Index
	
} //namespace ben

#endif
