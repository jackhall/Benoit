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
//#include "IndexBase.h" //included from Singleton.h
#include "Singleton.h" 

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

	//Singleton forward declares this
	template<typename S>
	class Index : public IndexBase {
	public:
		typedef S singleton_type;
		typedef typename S::id_type id_type;
		//iterator is not nested because I need to forward declare the output operator			
		class iterator;

	private:
		typedef IndexBase base_type;
		typedef Index self_type;
		//typedef std::unordered_map<id_type, singleton_type*> map_type;
		using base_type::index; //hiding this field
		using base_type::add;
		using base_type::remove;

		template<typename T>
		friend bool merge(std::shared_ptr<T> one, std::shared_ptr<T> two);
		virtual bool perform_merge(self_type& other) = 0; //should this still be a member function?

	protected:
		virtual ~Index() = default;

	public:	
		Index() = default;
		Index(const self_type& rhs) = delete; //identity semantics
		Index& operator=(const self_type& rhs) = delete; 
		Index(self_type&& rhs) = delete;
		Index& operator=(self_type&& rhs) = delete;
	
		//constness of this pointer preventing creation of nonconst iterator	
		iterator find(const id_type address) const { 
		//this const cast is ok because Index does not own the Singletons is manages
			return iterator( index.find(address) ); 
		}
		singleton_type& elem(const id_type address) const {
		//throw an exception if address does not exist?
		//this is not safe to use unless you already know that address exists in this index
			auto iter = index.find(address);
			return *static_cast<singleton_type*>(iter->second);
		}
		
		iterator begin() const { return iterator( index.begin() ); }
		iterator end() const { return iterator( index.end() ); }
	}; //class Index
	
	
	template<typename U> 
	std::ostream& operator<<(std::ostream& out, const typename Index<U>::iterator& iter);

	template<typename S>
	class Index<S>::iterator : public std::iterator<std::forward_iterator_tag, singleton_type> {
	protected:
		typename map_type::iterator current;
		friend class Index;
		friend std::ostream& operator<< <S>(std::ostream& out, const iterator& iter);
		iterator(const typename map_type::iterator iter)
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
		
		bool operator==(const iterator& rhs) const
			{ return current==rhs.current; }
		bool operator!=(const iterator& rhs) const
			{ return !( (*this) == rhs ); }
	}; //class iterator

	template<typename U>
	std::ostream& operator<<(std::ostream& out, const typename Index<U>::const_iterator& iter) {
		out << iter.current;
		return out;
	}

	//Singleton forward declares this
	template<typename T>
	bool merge(std::shared_ptr<T> one, std::shared_ptr<T> two) {
	//transfers management of all Singletons to other, first checking for redundancy
	//returns false if any Singletons had redundant IDs in other (reassign ID?), true else
	//either transfers all Singletons or none
	//this method can be used to emulate move semantics
		typedef typename T::singleton_type singleton_type;
		static_assert(std::is_base_of<Index<singleton_type>, T>::value,
				"Only Index-derived classes can be used");

		if(two == one) return false; //redundant, but clear
		if(two->size() == 0) return true; 
		for(auto x : two->index) if( one->manages(x.first) ) return false;
	
		//begin merge, leaving the process reversible
		auto it = two->index.begin(), ite = two->index.end();
		auto self_ptr = static_cast<singleton_type*>(it->second)->get_index(); 
		while(it != ite) {
			//does not call add or remove!
			one->index.insert( std::make_pair(it->first, it->second) );
			it->second->update_index(one);
			++it;
		}

		//need the explicit up-cast because derived classes are not friended	
		bool status = std::static_pointer_cast< Index<singleton_type> >(one)->perform_merge(*two);
	
		it = two->index.begin();
		if(status) two->index.erase(it, ite); //finish merge
		else {
			//reverse merge
			while(it != ite) {
				one->index.erase(it->first);
				it->second->update_index(self_ptr);
				++it;
			}
		}

		return status;
	}
	
} //namespace ben

#endif
