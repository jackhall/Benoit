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

	template<typename... ARGS> struct ConstructionTypes {};


	template<typename P, typename... ARGS>
	struct LinkManagerHelper {
		static_assert(!std::is_same<typename P::construction_types, ARGS...>::value, 
				"LinkManagerHelper needs an instance of ConstructionTypes; use LinkManager");
	};

	
	template<typename P, typename... ARGS>
	struct LinkManagerHelper< P, ConstructionTypes<ARGS...> > { 
	public:
		typedef P link_type;
		typedef typename link_type::complement_type link_complement_type;
		typedef typename P::id_type id_type;
		typedef LinkManagerHelper<typename link_type::complement_type, ConstructionTypes<ARGS...> > complement_type;
		typedef typename std::vector<link_type>::iterator iterator; 
		typedef typename std::vector<link_type>::const_iterator const_iterator;
		
	private:
		typedef LinkManagerHelper self_type;
		friend class LinkManagerHelper<typename link_type::complement_type, ConstructionTypes<ARGS...> >;
		std::vector<link_type> links;
	
	public:
		id_type nodeID;

		LinkManagerHelper() = default;
		LinkManagerHelper(const id_type id) : nodeID(id) {}
		LinkManagerHelper(const self_type& rhs) = delete;
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
			auto it = begin();
			for(; it!=end(); ++it)
				if(it->get_address() == address) break;
			return it;
		}
		const_iterator find(const id_type address) const {
			return const_cast<self_type*>(this)->find(address);
		}
		bool add(complement_type& other, const ARGS... args) {
			if( contains(other.nodeID) ) return false;
			links.push_back( link_type(other.nodeID, args...) );
			other.links.push_back( link_complement_type(links.back(), nodeID) );
			return true;
		}
		void add_clone_of(const link_type& x, complement_type& other) {
			links.push_back(x.clone());
			other.links.push_back( link_complement_type(links.back(), nodeID) );
		}
		void remove(complement_type& other, const iterator iter) {
			auto address = iter->get_address();
			assert(other.nodeID == address); //throw an exception?
			links.erase(iter);
			other.clean_up(nodeID);
		}
		void remove(complement_type& other) {
			auto iter = find(other.nodeID);
			if(iter != end()) remove(other, iter);
		}
		void clean_up(const id_type address) {
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

	//this alias significantly cleans up the interface, and makes it safer
	template<typename P>
	using LinkManager = LinkManagerHelper<P, typename P::construction_types>;

} //namespace ben

#endif

