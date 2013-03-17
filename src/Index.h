#ifndef BenoitIndex_h
#define BenoitIndex_h

/*
    Benoit: a flexible framework for distributed graphs and spaces
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

#include <unordered_map>
#include <memory>
#include <iostream>
#include "IndexBase.h"
#include "Commons.h"

namespace ben {
/* Index is a layer between IndexBase and any client code that provides a type-safe interface
 * specific to the type of Singleton it tracks. Static casting allows the general-purpose interface to
 * be adapted for any derived type of Singleton-Index abstraction. This way IndexBase can be reused without 
 * requiring client code to violate type safety. The interface of Index is designed to look like
 * an STL interface, complete with iterators and add, empty, size and find methods. I use "remove"
 * instead of "erase" because the Singleton's destructor is not called. The swap_with and merge_into
 * methods are not required for completeness, but should be convenient. 
 *
 * Index has access to the machinery of IndexBase, but hides it from any further-derived children
 * to protect the encapsulation of IndexBase. Semantics are based on identity, not value, so no copying is
 * allowed, although moves are. 
 *
 * Because IndexBase is now based on std::unordered_map, only forward iterators are provided.
 */
	template<typename S>
	class Index : public wayne::Commons {
	public:
		typedef S singleton_type;
		//iterators are not nested because I need to forward declare their output operator			
		class const_iterator; //do away with const_iterator? semantically weird
		class iterator;

	private:
		typedef Index		self_type;
		typedef typename S::id_type id_type;
		typedef std::unordered_map<id_type, Singleton*> map_type;
		
		friend singleton_type;
		bool update_singleton(singleton_type* ptr) {
		//updates the tracking for the indicated Singleton
		//returns false if no Singleton with this ID is currently being tracked, true otherwise
			auto iter = index.find(ptr->ID());
			if(iter != index.end()) { 
				iter->second = ptr;
				return true;
			} else return false;
		}

		map_type index;	
	
		bool add(singleton_type* ptr) { 
		//begins tracking referent of ptr
		//returns false if this Index is already tracking a Singleton with ptr's ID, true otherwise
			if( index.insert(std::make_pair(ptr->ID(), ptr)).second ) {
				bool status = perform_add(singleton_type); 
				if(!status) index.erase(ptr->ID());
				return status;
			} return false;
		 }
		bool remove(const id_type address) {
		//stops tracking Singleton with ID=address, 
		//returns false it this Index is not tracking a Singleton with ID=address, true otherwise
			auto iter = index.find(address);
			if( iter != index.end() ) {
				bool status = perform_remove(iter); 
				if(status) index.erase(iter);
				return status;
			} else return false;
		}
		
		//these functions perform custom operations on derived data structure
		//for context, see the exposed add, remove, and merge_into methods
		virtual bool perform_add() { return true; }
		virtual bool perform_remove(iterator iter) { return true; }
		virtual bool perform_merge(self_type& other) { return true; } 

	protected:
		virtual ~Index() { //should never be called while any Singletons are still managed
			//but for safety's sake...
			for(auto x : index) x.second->update_index(std::shared_ptr<self_type>()); 
		}

	public:	
		Index() = default;
		Index(const self_type& rhs) = delete; //identity semantics
		Index& operator=(const self_type& rhs) = delete; 
		Index(self_type&& rhs) = delete;
		Index& operator=(self_type&& rhs) = delete;
		
		bool contains(const id_type address) const { return index.count(address) == 1; }
		size_t size() const { return index.size(); }

		iterator find(const id_type address) const { return iterator( index.find(address) ); }
		singleton_type& elem(const id_type address) const {
		//throw an exception if address does not exist?
		//this is not safe to use unless you already know that address exists in this index
			auto iter = index.find(address);
			return *static_cast<singleton_type*>(iter->second);
		}
		bool check(const id_type address, const singleton_type* local_ptr) const { 
		//verifies correct tracking of Singleton
			auto iter = index.find(address);
			if(iter != index.end()) return iter->second == local_ptr;
			else return false;
		}
		
		bool merge_into(std::shared_ptr<self_type> other_ptr) { 
		//transfers management of all Singletons to other, first checking for redundancy
		//returns false if any Singletons had redundant IDs in other (reassign ID?), true else
		//either transfers all Singletons or none
		//this method can be used to emulate move semantics
			if(this == other_ptr.get()) return false; //redundant, but clear
			if(size() == 0) return true; 
			for(auto x : index) if( other_ptr->contains(x.first) ) return false;
		
			//begin merge, leaving the process reversible
			auto it = index.begin(), ite = index.end();
			auto self_ptr = it->second->get_index(); 
			while(it != ite) {
				//does not call add or remove!
				other_ptr->index.insert( std::make_pair(it->first, it->second) );
				it->second->update_index(other_ptr);
			}
		
			bool status = perform_merge(*other_ptr);
		
			it = index.begin();
			if(status) index.erase(it, ite); //finish merge
			else {
				//reverse merge
				while(it != ite) {
					other_ptr->index.erase(it->first);
					it->second->update_index(self_ptr);
				}
			}
	
			return status;
		}
		
		iterator begin() { return iterator( index.begin() ); }
		iterator end()   { return iterator( index.end() ); }
		const_iterator begin() const { return const_iterator( index.begin() ); }
		const_iterator end() const   { return const_iterator( index.end() ); }
	}; //class Index
	
	
	template<typename U> std::ostream& operator<<(std::ostream& out, const typename Index<U>::const_iterator& iter);

	template<typename S>
	class Index<S>::const_iterator : public std::iterator<std::forward_iterator_tag, singleton_type> {
	protected:
		typename map_type::iterator current;
		friend class Index;
		friend std::ostream& operator<< <S>(std::ostream& out, const const_iterator& iter);
		const_iterator(const typename map_type::iterator iter)
			: current(iter) {}
		const_iterator(const typename map_type::const_iterator iter)
			: current(iter) {}
			
	public:
		const_iterator() = default;
		const_iterator(const const_iterator& rhs) = default;
		const_iterator& operator=(const const_iterator& rhs) = default;
		virtual ~const_iterator() = default;
		
		const singleton_type& operator*() const 
			{ return *static_cast<singleton_type*>(current->second); } 
		const singleton_type* operator->() const 
			{ return static_cast<singleton_type*>(current->second); }
		
		const_iterator& operator++() { ++current; return *this; }
		const_iterator  operator++(int) { 
			auto temp = current;
			++current;
			return temp;
		}
		
		bool operator==(const const_iterator& rhs) const
			{ return current==rhs.current; }
		bool operator!=(const const_iterator& rhs) const
			{ return !( (*this) == rhs ); }
	}; //class const_iterator

	template<typename S>
	class Index<S>::iterator : public Index<S>::const_iterator {
	private:
		typedef const_iterator base_type;
		using base_type::current;
		friend class Index;
		iterator(const typename map_type::iterator iter)
			: base_type(iter) {}
		
	public:	
		singleton_type& operator*() const 
			{ return *static_cast<singleton_type*>(current->second); } 
		singleton_type* operator->() const 
			{ return static_cast<singleton_type*>(current->second); }
		
		iterator& operator++() { const_iterator::operator++(); }
		iterator  operator++(int) { 
			auto temp = current;
			++current;
			return temp;
		}
	}; //class iterator

	template<typename U>
	std::ostream& operator<<(std::ostream& out, const typename Index<U>::const_iterator& iter) {
		out << iter.current;
		return out;
	}

} //namespace ben

#endif
