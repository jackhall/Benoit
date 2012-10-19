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
		Index is a layer between IndexBase and any client code that provides a type-safe interface
		specific to the type of Singleton it tracks. This way IndexBase can be reused without 
		requiring client code to violate type safety. The interface of Index is designed to look like
		an STL interface, complete with iterators and add, empty, size and find methods. I use "remove"
		instead of "erase" because the Singleton's destructor is not called. The swap_with and merge_into
		methods are not required for completeness, but should be convenient. 
		
		Index has access to the machinery of IndexBase, but hides it from any further-derived children
		to protect the encapsulation of IndexBase. 
	*/

	template<typename S>
	class Index : public IndexBase {
	private:
		typedef S 		singleton_type;
		typedef IndexBase 	base_type;
		typedef Index		self_type;
		typedef size_t		size_type;
		
		using base_type::update_singleton;
		using base_type::update_all;
		using base_type::index; //FIELD
			
	public:				
		class iterator;
		class const_iterator;
	
		Index()=default;
		Index(const self_type& rhs) = delete;
		Index(self_type&& rhs) : base_type( std::move(rhs) ) {} //use move semantics to transfer all Nodes
		Index& operator=(const self_type& rhs) = delete; //make unique copy of all Nodes? no
		Index& operator=(self_type&& rhs) { base_type::operator=( std::move(rhs) ); }
		virtual ~Index() { clear(); }
		
		iterator find(const id_type address) { return iterator( index.find(address) ); }
		singleton_type& elem(const id_type address) const //throw if address does not exist?
			{ return *static_cast<singleton_type*>(index.find(address)->second); }
		bool check(const id_type address, const singleton_type* local_ptr) const 
			{ return base_type::check(address, local_ptr); }
		bool empty() const { return index.empty(); }
		
		virtual bool add(singleton_type& x) { return base_type::add(x); }
		size_type size() { return index.size(); }
		
		virtual void swap_with(self_type& other) { std::swap(index, other.index); }
		
		virtual bool merge_into(self_type& other) {
		//transfers management of all Singletons to other, first checking for redundancy
		//returns false if any Singletons had redundant IDs in other (reassign ID?), true else
		//either transfers all Singletons or none
			for(auto it=index.begin(), ite=index.end(); it!=ite; ++it) 
				if( other.contains(it->first) ) return false;
		
			for(auto it=index.begin(), ite=index.end(); it!=ite; ++it) 
				if( other.add(*static_cast<singleton_type*>(it->second)) ) index.erase(it);
		
			return true;
		}
		
		iterator begin() { return iterator( index.begin() ); }
		iterator end()   { return iterator( index.end() ); }
		const_iterator cbegin() { return const_iterator( index.cbegin() ); }
		const_iterator cend()   { return const_iterator( index.cend() ); }
	}; //class Index
	
	
	template<typename T>
	bool operator==(const typename Index<T>::const_iterator& lhs, 
			const typename Index<T>::const_iterator& rhs);
	
	template<typename S>
	class Index<S>::const_iterator : public std::iterator<std::bidirectional_iterator_tag, singleton_type> {
	private:
		typename std::map<id_type, Singleton*>::const_iterator current;
		friend class Index;
		friend bool operator==<S>(const const_iterator& lhs, const const_iterator& rhs);
		const_iterator(const typename std::map<id_type, Singleton*>::const_iterator iter)
			: current(iter) {}
		
	public:
		const_iterator() = default;
		const_iterator(const const_iterator& rhs) = default;
		const_iterator& operator=(const const_iterator& rhs) = default;
		~const_iterator() = default;
		
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
		const_iterator& operator--() { --current; return *this; }
		const_iterator  operator--(int) {
			auto temp = current;
			--current;
			return temp;
		}
		
		/*bool operator==(const const_iterator& rhs) const //compare iterator locations
			{ return current==rhs.current; }
		bool operator!=(const const_iterator& rhs) const
			{ return current!=rhs.current; }*/
	}; //class const_iterator
	
	
	template<typename S>
	class Index<S>::iterator : public std::iterator<std::bidirectional_iterator_tag, singleton_type> {
	private:
		typename std::map<id_type, Singleton*>::iterator current; //already inheriting pointer?
		friend class Index;
		iterator(const typename std::map<id_type, Singleton*>::iterator iter)
			: current(iter) {}
		
	public:	
		operator const_iterator() const { return const_iterator(current); }
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
		
		/*bool operator==(const iterator& rhs) const //compare iterator locations
			{ return current==rhs.current; }
		bool operator==(const const_iterator& rhs) const 
			{ return 
		bool operator!=(const iterator& rhs) const
			{ return current!=rhs.current; }*/
	}; //class iterator
	
	
	template<typename T>
	bool operator==(const typename Index<T>::const_iterator& lhs, 
			const typename Index<T>::const_iterator& rhs) {
		return lhs.current == rhs.current;
	}
	
	template<typename T>
	bool operator!=(const typename Index<T>::const_iterator& lhs, 
			const typename Index<T>::const_iterator& rhs) {
		return !(lhs == rhs);
	}
	
} //namespace ben

#endif
