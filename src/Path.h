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

namespace ben {

	template<typename V, typename N>
	struct ValueLink {
		std::atomic<V> value;
		N::id_type first_id, second_id;
		std::atomic<N*> first_ptr, second_ptr;
	};

	template<typename V, template<typename, typename> N>
	class Path {
	public:	
		typedef V value_type;
		typedef Path self_type;
		typedef Path complement_type;
		typedef N<self_type, self_type> node_type;
		
	private:
		std::shared_ptr< ValueLink<value_type, node_type> > link;
		node_type::id_type otherID;

	public:
		Path() = delete;
		Path(const id_type address); //ctors need work, need to match Port ctors
		Path(self_type& other, const id_type address);
		Path(const self_type& rhs);
		self_type& operator=(const self_type& rhs);
		~Path() = default;

		value_type get_value() const { return link->value.load(); }
		void set_value(const value_type& new_value) { link->value.store(new_value); }
		node_type& walk() const { 
			if(link->first_id == otherID) return *(link->first_ptr.load());
			else return *(link->second_ptr.load());
		}
		void update(node_type* new_ptr) {
			if(link->first_id == otherID) link->second_ptr.store(new_ptr);
			else link->first_ptr.store(new_ptr);
		}
	};
}; //namespace ben
