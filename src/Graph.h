#ifndef BenoitGraph_h
#define BenoitGraph_h

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
		Graph is a manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. It does not own the Nodes (or it would not be a distributed structure), but it stores pointers to them 
		in an STL map, using the Node ID as the key value. 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		Each Graph will have both a read and a write mutex when multithreading is implemented.
	*/
	
	template<typename N> 
	class Graph : public Index<N> {
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
		Graph()=default;
		Graph(const Graph& rhs) = delete;
		Graph(Graph&& rhs); //use move semantics to transfer all Nodes
		Graph& operator=(const Graph& rhs) = delete; //make unique copy of all Nodes? no
		Graph& operator=(Graph&& rhs);
		~Graph(); //transfers all Nodes to that class's static Graph
		
		reference find(const id_type address) const;
		bool contains(const id_type address) const;
		
		bool add(node_type& node); //only called by Node constructor
		void remove(const id_type address);  //does not remove Node's Graph pointer (for now)
		bool update_node(node_type& node); //makes sure if Node is listed and has an up-to-date pointer
		size_type size() { return IDMap.size(); }
		
		void move_to(Graph& destination, const id_type address); //move individual Node
		void swap_with(Graph& other); //all Nodes
		void merge_into(Graph& other); 
		
		class iterator : public std::iterator<std::bidirectional_iterator_tag, node_type> {
		private:
			typename std::map<id_type, pointer>::iterator current;
			friend class Graph;
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
	}; //class Graph
	
	template<typename N> 
	void Graph<N>::update_all();
	
	template<typename N> 
	reference Graph<N>::find(const id_type address) const;
	
	template<typename N> 
	bool Graph<N>::contains(const id_type address) const;
	
	template<typename N> 
	bool Graph<N>::add(node_type& node); 
	
	template<typename N> 
	void Graph<N>::remove(const id_type address);  
	
	template<typename N> 
	bool Graph<N>::update_node(node_type& node); 
	
	template<typename N> 
	void Graph<N>::move_to(Graph& destination, const id_type address); 
	
	template<typename N> 
	void Graph<N>::swap_with(Graph& other); 
	
	template<typename N> 
	void Graph<N>::merge_into(Graph& other); 
	
} //namespace ben

#endif
