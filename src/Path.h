#ifndef BenoitPath_h
#define BenoitPath_h

/*
    Benoit: a flexible framework for distributed graphs and spaces
    Copyright (C) 2011-2012  Jack Hall

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

#include <memory>
#include <atomic>
#include "LinkManager.h"

namespace ben {

	template<typename V> class Path;

	//untested
	template<typename V> bool operator==(const Path<V>& lhs, const Path<V>& rhs);
	template<typename V> bool operator!=(const Path<V>& lhs, const Path<V>& rhs);

	template<typename V>
	class Path {
/* Value-storing counterpart to Ports. Directionality is given only by the Node-level interface.
 * Any replacement must have:
 * 	id_type typedef
 * 	complement_type typedef
 * 	construction_types typedef
 * 	get_address() method
 * 	clone() method
 * 	Constructor(id_type, construction_types...)
 * 	Constructor(complement_type, id_type)
 * 	copy construction/assignment (for the STL)
 */
	public:	
		typedef V value_type;
		typedef Path complement_type; //even for directed graphs, value links are symmetric
		typedef unsigned int id_type;
		typedef ConstructionTypes<value_type> construction_types; //tells LinkManager how to construct it
		
	private:
		typedef Path self_type;
		id_type otherID;
		std::shared_ptr< std::atomic<value_type> > value_ptr;

		friend bool operator==<V>(const self_type& lhs, const self_type& rhs);
		friend bool operator!=<V>(const self_type& lhs, const self_type& rhs);

	public:
		Path() = delete;
		//1st ctor: new link, new shared_ptr
		//2nd ctor: matches existing complement, old shared_ptr
		Path(const id_type address, const value_type v) : otherID(address), value_ptr(new std::atomic<value_type>(v)) {} 
		Path(complement_type& other, const id_type address) : otherID(address), value_ptr(other.value_ptr) {}
		Path(const self_type& rhs) = default;
		self_type& operator=(const self_type& rhs) = default;
		Path(self_type&& rhs) : otherID(rhs.otherID), value_ptr( std::move(rhs.value_ptr) ) {}
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				otherID = rhs.otherID;
				value_ptr = std::move(rhs.value_ptr);
			}
		}
		~Path() = default;

		self_type clone(const id_type address) const { 
			//how to get a link w/new shared_ptr
			//can't use otherID because links-to-self would be impossible to clone
			return self_type(address, value_ptr->load()); 
		}

		bool is_ghost() const { return value_ptr.use_count() < 2; } //necessary but not sufficient condition for ghost
		id_type get_address() const { return otherID; } //required by all Port or Path types
		value_type get_value() const { return value_ptr->load(); }
		void set_value(const value_type& v) { value_ptr->store(v); }
	}; //class Path

	template<typename V>
	bool operator==(const Path<V>& lhs, const Path<V>& rhs) { return lhs.value_ptr == rhs.value_ptr; }
	template<typename V>
	bool operator!=(const Path<V>& lhs, const Path<V>& rhs) { return !operator==(lhs, rhs); }	

}; //namespace ben

#endif
