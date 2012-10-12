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

	class Singleton;

	class Index : public wayne::Commons {
	public:
		typedef unsigned int id_type;
		
	private:
		typedef Singleton 	singleton_type;
		typedef Singleton* 	pointer;
		typedef Singleton&	reference;
		typedef size_t	size_type;	
	
	protected:
		friend class Singleton;
	
		std::map<id_type, pointer> index;
		
		bool update_singleton(pointer x); //makes sure *x is listed and has an up-to-date pointer
		void update_all() const;
		
		Index()=default;
		Index(const Index& rhs) = delete;
		Index(Index&& rhs); //use move semantics to transfer all Nodes
		Index& operator=(const Index& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(Index&& rhs);
		virtual ~Index() { clear(); }
		
		reference find(const id_type address) const { return *(index.find(address)->second); }
		bool contains(const id_type address) const { return index.count(address) == 1; }
		bool check(const id_type address, const point_type* local_ptr) const 
			{ return index.find(address)->second == local_ptr; }
		bool empty() const { return index.empty(); }
		
		bool add(reference x);
		bool remove(const id_type address);
		void clear();
		size_type size() { return index.size(); }
		
		bool move_to(Index& other, const id_type address); //move individual Singleton
		void swap_with(Index& other) { std::swap(index, other.index); }
		bool merge_into(Index& other); 
		
		class iterator : public std::iterator<std::bidirectional_iterator_tag, node_type> {
		private:
			typename std::map<id_type, pointer>::iterator current;
			friend class Index;
			iterator(const typename std::map<id_type, pointer>::iterator iter)
				: current(iter) {}
			
		public:
			iterator() = default;
			iterator(const iterator& rhs) = default;
			iterator& operator=(const iterator& rhs) = default;
			~iterator() = default;
			
			reference operator*() const { return *(current->second); } 
			pointer operator->() const { return current->second; }
			
			iterator& operator++() { ++current; return *this; }
			iterator  operator++(int) { 
				auto temp = current;
				++current;
				return temp;
			}
			iterator& operator--() { --current; return *this; }
			iterator  operator--(int) {
				auto temp = current;
				--current;
				return temp;
			}
			
			bool operator==(const iterator& rhs) const //compare iterator locations
				{ return current==rhs.current; }
			bool operator!=(const iterator& rhs) const
				{ return current!=rhs.current; }
		}; //class iterator
			
		iterator begin() { return iterator( IDMap.begin() ); }
		iterator end()   { return iterator( IDMap.end() ); }
	}; //class Index
	
} //namespace ben

#endif
