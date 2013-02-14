#ifndef BenoitUndirectedNode_h
#define BenoitUndirectedNode_h

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

#include <vector>
#include "Path.h"

namespace ben {
	
	template<typename V>
	class UndirectedNode : public Singleton { 
	private:
		typedef UndirectedNode self_type;
		typedef Singleton base_type;
	
	public:
		typedef Graph<UndirectedNode> index_type;
		typedef V value_type;
		typedef Path<value_type> link_type;
		typedef typename LinkManager<link_type>::iterator iterator; 
		
	private:
		LinkManager<link_type> links;
	
	public:
		iterator find(const id_type address) { return links.find(address); } 

		template<typename... Args>
		bool add(const id_type address, Args... args) {
			static_assert(std::is_same< typename link_type::construction_types, ConstructionTypes<Args...> >::value,
					"extra arguments for UndirectedNode::add must match link_type::construction_types");
			auto node_iter = get_index().find(address);
			if(node_iter != get_index().end()) return links.add(node_iter->links, address, args...);
			else return false;
		}
	
		bool clone_links(const self_type& other) { //need a way to clone links
			//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
			if( other.is_managed_by(get_index()) ) { 
				clear();
				id_type currentID;
				for(const link_type& x : other.links) {
					currentID = x.get_address();
					if(currentID != ID()) add(currentID, x.get_value());
				}
				return true;
			} else return false;
		}

		void remove(const iterator iter) {
			auto node_iter = get_index.find(iter->get_address());
			links.remove(node_iter->links, iter);
		}

		void remove(const id_type address) {
			auto iter = find(address);
			if(iter != end()) remove(iter);
		}

		void clear() {
			for(link_type& x : links) get_index().elem(x.get_address()).links.clean_up(ID());
			links.clear();
		}
		
		size_t size() const { return links.size(); }

		bool contains(const id_type address) const { return links.contains(address); }
		
		self_type& walk(const iterator iter) const { return get_index().elem(iter->get_address()); }
		iterator begin() { return links.begin(); }
		iterator end() { return links.end(); }
	}; //class UndirectedNode

} //namespace ben

#endif

