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

#include <iostream>
#include <memory>
#include <atomic>
//#include "yaml-cpp/yaml.h"

namespace ben {

	template<typename VALUE>
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
		typedef Path complement_type; //even for directed graphs, value links are symmetric
		typedef unsigned int id_type;
		
	private:
		typedef Path self_type;
		id_type otherID;
		std::shared_ptr< std::atomic<VALUE> > value_ptr;

	public:
		Path() = delete;
		//1st ctor: new link, new shared_ptr
		//2nd ctor: matches existing complement, old shared_ptr
		Path(id_type address, const VALUE& v) 
            : otherID(address), value_ptr(new std::atomic<VALUE>(v)) {} 
		Path(complement_type& other, id_type address) 
            : otherID(address), value_ptr(other.value_ptr) {}
		Path(const self_type& rhs) = default;
		self_type& operator=(const self_type& rhs) = default;
		Path(self_type&& rhs) = default;
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
		VALUE get_value() const { return value_ptr->load(); }
		void set_value(const VALUE& v) { value_ptr->store(v); }

	    bool operator==(const self_type& rhs) const { return value_ptr == rhs.value_ptr; }
	    bool operator!=(const self_type& rhs) const { return !operator==(rhs); }	
	}; //class Path

}; //namespace ben
/*
namespace YAML {
	template<typename V> 
	struct convert< ben::Path<V> > {
		static Node encode(const ben::Path<V>& rhs) {
			Node node;
			node["address"] >> rhs.get_address();
			node["value"] >> rhs.get_value();
			return node;
		}

		static bool decode(const Node& node, ben::Path<V>& rhs) {
			if(!node.IsMap() || node["address"] || node["value"]) return false;
			if(node["address"] != rhs.get_address()) return false;	
			rhs.set_value(node["value"]);	
			return true;
		}
	};
}; //namespace YAML
*/
#endif
