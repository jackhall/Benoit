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

#include "Path.h"
#include "LinkManager.h"

namespace ben {
	
	template<typename P>
	class UndirectedNode : public Singleton { 
	private:
		typedef UndirectedNode self_type;
		typedef Singleton base_type;
	
	public:
		typedef Graph<UndirectedNode> index_type;
		typedef P link_type;
		typedef typename LinkManager<link_type>::iterator iterator; 
		
	private:
		static_assert(std::is_same<id_type, typename P::id_type>::value, 
			      "Index and Port unique ID types don't match");

		LinkManager<link_type> links;
	
	public:
		UndirectedNode() = default;
		explicit UndirectedNode(const id_type id) : base_type(id), links(id) {}
		explicit UndirectedNode(index_type& graph) : base_type(graph) {}
		UndirectedNode(index_type& graph, const id_type id) : base_type(graph, id), links(id) {}
		UndirectedNode(const self_type& rhs) = delete;
		UndirectedNode& operator=(const self_type& rhs) = delete;
		UndirectedNode(self_type&& rhs) : base_type(std::move(rhs)), links(std::move(rhs.links)) {}
		UndirectedNode& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=(std::move(rhs));
				links = std::move(rhs.links);
			}
		}
		virtual ~UndirectedNode() { clear(); } //might want to lock while deleting links

		using base_type::ID;
		const index_type& get_index() const 
			{ return static_cast<const index_type&>(base_type::get_index()); }
		using base_type::is_managed;
		bool is_managed_by(const index_type& x) const { return base_type::is_managed_by(x); }
		iterator find(const id_type address) { return links.find(address); } 

		template<typename... Args>
		bool add(const id_type address, Args... args) {
			static_assert(std::is_same< typename link_type::construction_types, ConstructionTypes<Args...> >::value,
					"extra arguments for UndirectedNode::add must match link_type::construction_types");
			auto node_iter = get_index().find(address);
			if(node_iter != get_index().end()) return links.add(node_iter->links, address, args...);
			else return false;
		}
	
		bool clone_links(const self_type& other) { 
			//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
			if( other.is_managed_by(get_index()) ) { 
				clear();
				id_type currentID;
				for(const auto& x : other.links) {
					currentID = x.get_address();
					if(currentID != ID()) {	
						auto& target = get_index().elem(currentID).links;
						links.add_clone_of(x, target)
					}
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
			for(auto& x : links) get_index().elem(x.get_address()).links.clean_up(ID());
			links.clear();
		}
		
		size_t size() const { return links.size(); }

		bool contains(const id_type address) const { return links.contains(address); }
		
		self_type& walk(const iterator iter) const { return get_index().elem(iter->get_address()); }
		iterator begin() { return links.begin(); }
		iterator end() { return links.end(); }
	}; //class UndirectedNode

	template<typename V>
	using stdUndirectedNode = UndirectedNode< Path<V> >;

} //namespace ben

#endif

