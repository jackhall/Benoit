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
	
	template<typename S> 
	class Index : public wayne::Commons {
	public:
		typedef S 	singleton_type;
		typedef S* 	pointer;
		typedef S&	reference;
		typedef size_t	size_type;
		typedef unsigned int id_type;
	protected:
		std::map<id_type, pointer> index;
		
		virtual bool update_singleton(pointer x); //makes sure *x is listed and has an up-to-date pointer
		void update_all() 
			{ std::for_each(begin(), end(), [](reference x) { x.update_index(this); }); }
		
		friend class Singleton<Index>;
		
	public:	
		Index()=default;
		Index(const Index& rhs) = delete;
		Index(Index&& rhs); //use move semantics to transfer all Nodes
		Index& operator=(const Index& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(Index&& rhs);
		virtual ~Index() { clear(); }
		
		reference find(const id_type address) const { return *(index.find(address)->second); }
		bool contains(const id_type address) const { return index.count(address) == 1; }
		
		virtual bool add(reference x) { 
			if( index.insert(std::make_pair(x.ID(), &x))->second ) {
				x.update_index(this); return true;
			} return false;
		}
		virtual bool remove(const id_type address);
		virtual void clear() { 
			std::for_each(begin(), end(), [](reference x) { x.update_index(nullptr); }); 
			index.clear(); 
		}
		size_type size() { return index.size(); }
		
		virtual bool move_to(Index& destination, const id_type address); //move individual Singleton
		virtual void swap_with(Index& other) { std::swap(index, other.index); }
		virtual bool merge_into(Index& other); 
		
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
	
	template<typename S>
	Index<S>::Index(Index&& rhs) 
		: index( std::move(rhs.index) ) { update_all(); }
	
	template<typename S>
	Index<S>& Index<S>::operator=(Index&& rhs) {
		if(this != &rhs) {
			clear();
			index = std::move(rhs.index);
			update_all();
		} return *this;
	}
	
	template<typename S> 
	bool Index<S>::update_singleton(pointer x) {
		auto iter = index.find(x->ID());
		if(iter != index.end()) { 
			iter->second = x;
			return true;
		} else return false;
	}
	
	template<typename S>
	bool Index<S>::remove(const id_type address) { 
		auto iter = index.find(address);
		if( iter != index.end() ) { 
			iter->second->update_index(nullptr); 
			index.erase(iter);
			return true;
		} else return false;
	}	
	
	template<typename S> 
	bool Index<S>::move_to(Index& destination, const id_type address) {
		auto iter = index.find(address);
		if(iter != index.end()) {
			if( destination.add(*(iter->second)) ) {
				it->second->update_index(&destination);
				return remove(address);
			} else return false; //redundant ID in destination
		} else return false; //no element with that ID here
	}
	
	template<typename S> 
	bool Index<S>::merge_into(Index& other) {
		//returns true if all singletons were transferred
		//returns false if any had redundant IDs in other (reassign ID?)
		auto it = index.begin();
		auto ite = index.end();
		while(it != ite) {
			if( other.add(*(it->second)) ) index.erase(it++); 
			else ++it;
		}
		return index.empty();
	}
	
} //namespace ben

#endif
