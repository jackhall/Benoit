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

	template<typename V>
	class Path {
	public:	
		typedef V value_type;
		typedef Path self_type;
		typedef Path complement_type;
		typedef unsigned int id_type;
		typedef ConstructionTypes<value_type> construction_types;
		
	private:
		id_type otherID;
		std::shared_ptr< std::atomic<value_type> > value_ptr;

	public:
		Path() = delete;
		Path(const id_type address, const value_type v) : otherID(address), value_ptr(new value_type(v)) {}
		Path(complement_type& other, const id_type address) : otherID(address), value_ptr(other.value_ptr) {}
		Path(const self_type& rhs) = default;
		self_type& operator=(const self_type& rhs) = default;
		~Path() = default;

		self_type clone() const { return self_type(otherID, value_ptr->load()); }

		id_type get_address() const { return otherID; }
		value_type get_value() const { return value_ptr->load(); }
		void set_value(const value_type& v) { value_ptr->store(v); }
	}; //class Path

}; //namespace ben

#endif
