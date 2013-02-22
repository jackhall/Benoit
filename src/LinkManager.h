#ifndef BenoitLinkManager_h
#define BenoitLinkManager_h

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
#include <cassert>

namespace ben {

	//the struct carries around its template argument pack, like a variadic typedef
	template<typename... ARGS> struct ConstructionTypes {};


	template<typename P, typename... ARGS>
	struct LinkManagerHelper {
		//Don't let this compile! Only the specialization taking a ConstructionTypes struct
		//should be used. The static_assert here doesn't quite say the right thing...
		static_assert(!std::is_same<typename P::construction_types, ARGS...>::value, 
				"LinkManagerHelper needs an instance of ConstructionTypes; use LinkManager");
	};

	
	template<typename P, typename... ARGS>
	struct LinkManagerHelper< P, ConstructionTypes<ARGS...> > { 
/* This helper class does all the LinkManager work, but has a messier and less safe template interface
 * than it should. I use a template typedef to hide this template interface under the name LinkManager.
 * LinkManager is part of the internal machinery of Node classes, allowing the reuse of much code for
 * handling ports/paths/links. Unlike the rest of Benoit, I do not recommend customizing or replacing this
 * class during normal use of the library. 
 *
 * When writing Node classes, remember to initialize LinkManagers with the node's ID. When writing Port
 * classes, remember to typedef an instance of ConstructionTypes as construction_types. 
 */
	public:
		typedef P link_type;
		typedef typename link_type::complement_type link_complement_type;
		typedef typename P::id_type id_type;
		typedef LinkManagerHelper<typename link_type::complement_type, ConstructionTypes<ARGS...> > complement_type;
		typedef typename std::vector<link_type>::iterator iterator; 
		typedef typename std::vector<link_type>::const_iterator const_iterator;
		
	private:
		typedef LinkManagerHelper self_type;
		friend class LinkManagerHelper<link_complement_type, ConstructionTypes<ARGS...> >; //for noncircular calls to add/remove
		std::vector<link_type> links; 

	public:
		id_type nodeID; //needed to initialize complement Ports, public because LinkManager is internal to Node

		LinkManagerHelper() = delete;
		LinkManagerHelper(const id_type id) : nodeID(id) {}
		LinkManagerHelper(const self_type& rhs) = delete; //identity semantics
		self_type& operator=(const self_type& rhs) = delete;
		LinkManagerHelper(self_type&& rhs) : nodeID(rhs.nodeID), links(std::move(rhs.links)) {}
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				nodeID = rhs.nodeID;
				links = std::move(rhs.links);
			}
		} 
		~LinkManagerHelper() = default;

		iterator find(const id_type address) {
			//Using a sequence container means that this method takes O(n) operations,
			//but using vector to store ports may allow cache optimizations in the compiler.
			auto it = begin();
			for(; it!=end(); ++it)
				if(it->get_address() == address) break;
			return it;
		}
		const_iterator find(const id_type address) const {
			return const_cast<self_type*>(this)->find(address);
		}
		bool add(complement_type& other, const ARGS... args) {
			if( contains(other.nodeID) ) return false; //there is already a link to this other Node/LinkManager
			links.push_back( link_type(other.nodeID, args...) );
			other.links.push_back( link_complement_type(links.back(), nodeID) ); 
			return true;
		}
		void add_clone_of(const link_type& x, complement_type& other) {
			//for copying the links of a Node
			//this member does little work because undirected links can't be treated
			//the same as directed links
			links.push_back(x.clone(other.nodeID));
			other.links.push_back( link_complement_type(links.back(), nodeID) ); //link-to-self
		}
		bool add_self_link(const ARGS... args) {
			//without this, UndirectedNodes either end up violating encapsulation of LinkManager
			//or having two copies of every link-to-self
			if( contains(nodeID) ) return false;
			links.push_back( link_type(nodeID, args...) );
			return true;
		}
		void add_self_link_clone_of(const link_type& x) {
			//without this, UndirectedNodes either end up violating encapsulation of LinkManager
			//or having two copies of every link-to-self
			links.push_back( x.clone(nodeID) );
		}
		void remove(complement_type& other, const iterator iter) {
			//remove deletes a port and its complement
			auto address = iter->get_address();
			assert(other.nodeID == address); //throw an exception?
			links.erase(iter);
			other.clean_up(nodeID);
		}
		void remove(complement_type& other) {
			auto iter = find(other.nodeID);
			if(iter != end()) remove(other, iter); //else throw? 
		}
		void clean_up(const id_type address) {
			//removes a link independently of its complement
			//Since the cleanup methods are called by remove, delegating doesn't work.
			//The possibility of copying ports means that reference counting the links
			//does not necessarily work for identifying ghost links. Would there be a
			//performance hit for using weak_ptr in the copies? Is this safe?
			auto iter = find(address);
			if(iter != end()) links.erase(iter);
		}

		void clear() { links.clear(); } //does not clean up after links!
		
		size_t size() const { return links.size(); } 
		bool contains(const id_type address) const { return links.end() != find(address); }
		
		iterator begin() { return links.begin(); }
		const_iterator begin() const { return links.begin(); }
		iterator end() { return links.end(); }
		const_iterator end() const { return links.end(); }
	}; //class LinkManager

	//this alias significantly cleans up the interface and makes it safer
	template<typename P>
	using LinkManager = LinkManagerHelper<P, typename P::construction_types>;

} //namespace ben

#endif

