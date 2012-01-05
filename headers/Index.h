#ifndef BenoitIndex_h
#define BenoitIndex_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <map>
#include <iostream>
//#include <mutex>

namespace ben {

	template<typename W, typename S> class Node;

	/*
		Index is a manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. It does not own the Nodes (or it would not be a distributed structure), but it stores pointers to them 
		in an STL map, using the Node ID as the key value. 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		Each Index will have both a read and a write mutex when multithreading is implemented.
	*/
	
	template<typename W, typename S> 
	class Index {
	
	private:
		//std::mutex readLock, writeLock;
		//set up muticies so that a write lock is exclusive, but a read lock is not. A read lock
		//only prevents writing. 
		
		std::map< unsigned int, Node<W,S>* > IDMap; 
		
	public:
		Index()=default;
		~Index(); //transfers all Nodes to that class's static Index
		Index(const Index& rhs) = delete; //allow copying of whole network? 
		Index& operator=(const Index& rhs) = delete; //assign whole network?
		Node<W,S>* find(const unsigned int address) const;
		bool contains(const unsigned int address) const;
		
		void add(Node<W,S>* const pNode); //only called by Node constructor?
		void remove(const unsigned int address);  //does not remove Node's Index pointer (for now)
		bool update(Node<W,S>* const pNode); //makes sure if Node is listed and has an up-to-date pointer
		
		void move_to(Index& destination, const unsigned int address); //move individual Node
		void swap_with(Index& other); //all Nodes
		void merge_into(Index& other); 
			
	}; //class Index
	
} //namespace ben

#endif
