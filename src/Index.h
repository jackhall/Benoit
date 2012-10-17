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
#include "IndexBase.h"

namespace ben {
	/*
		Index is a manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. It does not own the Nodes (or it would not be a distributed structure), but it stores pointers to them 
		in an STL map, using the Node ID as the key value. 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		Each Index will have both a read and a write mutex when multithreading is implemented.
	*/

	template<typename S>
	class Index : public IndexBase {
	private:
		typedef S 		singleton_type;
		typedef IndexBase 	base_type;
		typedef Index		self_type;
		typedef size_t		size_type;
			
	public:				
		Index()=default;
		Index(const self_type& rhs) = delete;
		Index(self_type&& rhs) : base_type( std::move(rhs) ) {} //use move semantics to transfer all Nodes
		Index& operator=(const self_type& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(self_type&& rhs) { base_type::operator=( std::move(rhs) ); }
		virtual ~Index() { clear(); }
		
		singleton_type& find(const id_type address) const //throw if address does not exist?
			{ return *static_cast<singleton_type*>(index.find(address)->second); }
		bool check(const id_type address, const singleton_type* local_ptr) const 
			{ return base_type::check(address, local_ptr); }
		bool empty() const { return index.empty(); }
		
		virtual bool add(singleton_type& x) { return base_type::add(x); }
		size_type size() { return index.size(); }
		
		virtual bool move_to(self_type& other, const id_type address) { //move individual Singleton
			auto iter = index.find(address);
			if(iter != index.end()) {
				if( other.add(*static_cast<singleton_type*>(iter->second)) ) {
					index.erase(iter);
					return true;
				} else return false; //redundant ID in destination
			} else return false; //no element with that ID here
		}
		
		virtual void swap_with(self_type& other) { std::swap(index, other.index); }
		
		virtual bool merge_into(self_type& other) {
			//returns true if all singletons were transferred
			//returns false if any had redundant IDs in other (reassign ID?)
			for(auto it=index.begin(), ite=index.end(); it!=ite; ++it) 
				if( other.add(*static_cast<singleton_type*>(it->second)) ) index.erase(it);
			
			return index.empty();
		}
		
		class iterator : public std::iterator<std::bidirectional_iterator_tag, singleton_type> {
		private:
			typename std::map<id_type, Singleton*>::iterator current;
			friend class Index;
			iterator(const typename std::map<id_type, Singleton*>::iterator iter)
				: current(iter) {}
			
		public:
			iterator() = default;
			iterator(const iterator& rhs) = default;
			iterator& operator=(const iterator& rhs) = default;
			~iterator() = default;
			
			singleton_type& operator*() const 
				{ return *static_cast<singleton_type*>(current->second); } 
			singleton_type* operator->() const 
				{ return static_cast<singleton_type*>(current->second); }
			
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
			
		iterator begin() { return iterator( index.begin() ); }
		iterator end()   { return iterator( index.end() ); }
	}; //class Index
	
} //namespace ben

#endif
