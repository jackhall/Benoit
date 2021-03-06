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

#include "Singleton.h"
#include "Path.h"
#include "LinkManager.h"

namespace ben {
	
	template<typename P>
	class UndirectedNode : public Singleton { 
/* UndirectedNode is the counterpart to DirectedNode. Between them, it should be possible to represent
 * any sort of distributed graph by choosing the proper (possibly custom) Path or Port classes. 
 *
 * The standard type of UndirectedNode holds a templated value in each link, shared atomically by the
 * two nodes the link connects. As in DirectedNode, most of the work is delegated to LinkManager. See
 * DirectedNode for a little more information.
 */
	private:
		typedef UndirectedNode self_type;
		typedef Singleton base_type;
	
	public:
		typedef Graph<UndirectedNode> index_type;
		typedef P link_type;
		typedef typename LinkManager<self_type, link_type>::const_iterator const_iterator;
		typedef typename LinkManager<self_type, link_type>::iterator iterator; 
		
	private:
		static_assert(std::is_same<id_type, typename P::id_type>::value, 
			      "Index and Path unique ID types don't match");

		LinkManager<self_type, link_type> links;
		//std::mutex

		void perform_leave() { clear(); }
	
	public:
		UndirectedNode() : base_type(), links(ID()) {}
		explicit UndirectedNode(const id_type id) : base_type(id), links(id) {}
		explicit UndirectedNode(std::shared_ptr<index_type> graph) : base_type(graph), links(ID()) {}
		UndirectedNode(std::shared_ptr<index_type> graph, const id_type id) : base_type(graph, id), links(id) {}
		UndirectedNode(const self_type& rhs) = delete; //identity semantics
		UndirectedNode& operator=(const self_type& rhs) = delete;
		UndirectedNode(self_type&& rhs) : base_type(std::move(rhs)), links(std::move(rhs.links)) {}
		UndirectedNode& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=(std::move(rhs));
				links = std::move(rhs.links);
			}
		}
		~UndirectedNode() { clear(); } //might want to lock while deleting links

		bool join_index(std::shared_ptr<index_type> ptr) { return base_type::join_index(ptr); }
		std::shared_ptr<index_type> get_index() const //ensures proper type casting of the index pointer 
			{ return std::static_pointer_cast<index_type>(base_type::get_index()); }

		iterator find(const id_type address) { return links.find(address); } 
		const_iterator find(const id_type address) const { return links.find(address); }

		template<typename... Args>
		bool add(const id_type address, Args... args) {
			//returns false if the address does not exist or a link is already there
			//add should only be instantiated with arguments matching the Path constructor
			static_assert(std::is_same< typename link_type::construction_types, ConstructionTypes<Args...> >::value,
					"extra arguments for UndirectedNode::add must match link_type::construction_types");
			auto node_iter = get_index()->find(address); 
			if(node_iter != get_index()->end()) return links.add(node_iter->links, args...);
			else return false;
		}
		bool mirror(const self_type& other) { 
			//links-to-self are cloned to preserve the pattern - if other has a link-to-self, then
			//this will also have a link-to-self, not a link to other 
			if( get_index() == other.get_index() ) {
				auto path_iter = find(other.ID());
				if(path_iter != end()) { //if other contains a link to this node...
					auto temp = *path_iter;
					for(auto iter=begin(); iter!=end(); ++iter) 
						if(iter->get_address() != other.ID()) //clean up all except links to other
							walk(iter).links.clean_up(ID());
					links.clear();
					links.restore(temp, other.links);//add back the saved link 
				} else clear(); //would have deleted one of the links we're trying to copy

				for(const auto& x : other.links) {
					id_type currentID = x.get_address(); 
					if(currentID != ID()) { //if this link was there, it is left alone 
						if(currentID == other.ID()) links.add_self_link_clone_of(x); 
						else {
							auto& target = get_index()->elem(currentID);
							links.add_clone_of(x, target.links);
						}
					}
				}
				return true;
			} else return false;
		}
		void remove(const iterator iter) {
			//gets an iterator to the other node and lets LinkManager::remove do the rest
			//of the work
			auto node_iter = get_index()->find(iter->get_address());
			links.remove(node_iter->links, iter);
		}
		void remove(const id_type address) {
			//finds the link referred to and delegates to the other overload of remove
			//if the other link is not found, nothing happens 
			auto iter = find(address);
			if(iter != end()) remove(iter);
		}
		void clear() {
			//removes all link complements before deleting the local copy of the links, thereby
			//preventing iterator invalidation
			for(auto& x : links) get_index()->elem(x.get_address()).links.clean_up(ID());
			links.clear();
		}
		
		size_t size() const { return links.size(); }

		bool contains(const id_type address) const { return links.contains(address); }
		self_type& walk(const const_iterator iter) const { 
			//returns a reference to the node that iter points to: walks the graph
			//iterator is implicitly cast to const_iterator
			return get_index()->elem(iter->get_address()); 
		}
		iterator begin() { return links.begin(); }
		const_iterator begin() const { return links.begin(); }
		iterator end() { return links.end(); }
		const_iterator end() const { return links.begin(); }
	}; //class UndirectedNode

	//simply uses the default Path class, more user-friendly
	template<typename V>
	using stdUndirectedNode = UndirectedNode< Path<V> >;

} //namespace ben

#endif

