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
#include "Lock.h"

namespace ben {
	/*
		Index is a manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. It does not own the Nodes (or it would not be a distributed structure), but it stores pointers to them 
		in an STL map, using the Node ID as the key value. 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		Each Index will have both a read and a write mutex when multithreading is implemented.
	*/
	
	template<typename N> 
	class Index {
	public:
		typedef N 	node_type;
		typedef N* 	pointer;
		typedef N&	reference;
		typedef size_t	size_type;
		typedef typename N::id_type id_type;
	private:
		std::map<id_type, pointer> IDMap;
		
		void update_all();
		
	public:
		Commons lock; 
	
		Index()=default;
		Index(const Index& rhs) = delete;
		Index(Index&& rhs); //use move semantics to transfer all Nodes
		Index& operator=(const Index& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(Index&& rhs);
		~Index(); //transfers all Nodes to that class's static Index
		
		reference find(const id_type address) const;
		bool contains(const id_type address) const;
		
		bool add(node_type& node); //only called by Node constructor
		void remove(const id_type address);  //does not remove Node's Index pointer (for now)
		bool update_node(node_type& node); //makes sure if Node is listed and has an up-to-date pointer
		size_type size() { return IDMap.size(); }
		
		void move_to(Index& destination, const id_type address); //move individual Node
		void swap_with(Index& other); //all Nodes
		void merge_into(Index& other); 
		
		class iterator : public std::iterator<std::bidirectional_iterator_tag, node_type> {
		private:
			typename std::map<id_type, pointer>::iterator current;
			friend class Index;
			iterator(const typename std::map<id_type, pointer>::iterator iNode);
			
		public:
			iterator() = default;
			iterator(const iterator& rhs) = default;
			iterator& operator=(const iterator& rhs) = default;
			~iterator() = default;
			
			reference operator*() const { return *(current->second); } 
			pointer operator->() const { return current->second; }
			
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
		iterator end()   { return iterator( IDMap.end() ); }
	}; //class Index
	
	template<typename N> 
	void Index<N>::update_all();
	
	template<typename N> 
	reference Index<N>::find(const id_type address) const;
	
	template<typename N> 
	bool Index<N>::contains(const id_type address) const;
	
	template<typename N> 
	bool Index<N>::add(node_type& node); 
	
	template<typename N> 
	void Index<N>::remove(const id_type address);  
	
	template<typename N> 
	bool Index<N>::update_node(node_type& node); 
	
	template<typename N> 
	void Index<N>::move_to(Index& destination, const id_type address); 
	
	template<typename N> 
	void Index<N>::swap_with(Index& other); 
	
	template<typename N> 
	void Index<N>::merge_into(Index& other); 
	
} //namespace ben

#endif
