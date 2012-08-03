#ifndef BenoitIndex_h
#define BenoitIndex_h

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

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
		void update_all();
		
	public:
		Index()=default;
		~Index(); //transfers all Nodes to that class's static Index
		Index(const Index& rhs) = delete;
		Index(Index&& rhs); //use move semantics to transfer all Nodes
		Index& operator=(const Index& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(Index&& rhs);
		Node<W,S>* find(const unsigned int address) const;
		bool contains(const unsigned int address) const;
		
		bool add(Node<W,S>& pNode); //only called by Node constructor
		void remove(const unsigned int address);  //does not remove Node's Index pointer (for now)
		bool update_node(Node<W,S>& pNode); //makes sure if Node is listed and has an up-to-date pointer
		
		void move_to(Index& destination, const unsigned int address); //move individual Node
		void swap_with(Index& other); //all Nodes
		void merge_into(Index& other); 
		
		class iterator {
		private:
			typedef Node<W,S>& reference;
		
			typename std::map< unsigned int, Node<W,S>* >::iterator current;
			friend class Index;
			iterator(const typename std::map<unsigned int, Node<W,S>* >::iterator iNode);
		public:
			iterator() = default;
			iterator(const iterator& rhs) = default;
			iterator& operator=(const iterator& rhs) = default;
			~iterator() = default;
			
			Node<W,S>& operator*() const { return *(current->second); } 
			Node<W,S>* operator->() const { return current->second; }
			unsigned int address() { return current->first; }
			
			iterator& operator++();
			iterator  operator++(int);
			iterator& operator--();
			iterator  operator--(int);
			
			bool operator==(const iterator& rhs) const //compare iterator locations
				{ return current==rhs.current; }
			bool operator!=(const iterator& rhs) const
				{ return current!=rhs.current; }
		}; //class iterator
			
		iterator begin() { return iterator( IDMap.begin() ); }
		iterator end() { return iterator( IDMap.end() ); }
	}; //class Index
	
} //namespace ben

#endif
