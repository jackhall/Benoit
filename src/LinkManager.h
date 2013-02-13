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

namespace ben {

	template<typename... args> struct ConstructionTypes {};


	template<typename P, typename... ARGS>
	struct LinkManagerHelper {
		static_assert(!std::is_same<typename P::construction_types, Args...>::value, 
				"LinkManagerHelper needs an instance of ConstructionTypes; use LinkManager");
	};

	
	template<typename P, typename... ARGS>
	struct LinkManagerHelper< P, ConstructionTypes<ARGS...> > { 
	public:
		typedef P link_type;
		typedef typename P::id_type id_type;
		typedef LinkManagerHelper<typename link_type::complement_type> complement_type;
		typedef typename std::vector<link_type>::iterator iterator; 
		
	private:
		typedef LinkManagerHelper self_type;
		std::vector<link_type> links;
		void clean_up(const id_type address); 
	
	public:
		iterator find(const id_type address); 

		bool add(const id_type address, const ARGS... args); 
		bool clone(const self_type& other); 
		void remove(const iterator iter, complement_type& other);
		void remove(const id_type address, complement_type& other);
		void clear() { links.clear(); }
		
		size_t size() const { return links.size(); }
		bool contains(const id_type address) const { return links.end() != find(address); }
		
		iterator begin() { return links.begin(); }
		iterator end() { return links.end(); }
	}; //class LinkManager

	//this alias significantly cleans up the interface, and makes it safer
	template<typename P>
	using LinkManager = LinkManagerHelper<P, typename P::construction_types>;
	
	template<typename P, typename... ARGS>
	void LinkManager<P>::clean_up(const id_type address) {
		auto iter = find(address);
		if(iter != end()) links.erase(iter);
	}

	template<typename P, typename... ARGS>
	typename LinkManager<P>::iterator LinkManager<P>::find(const id_type address) {
		auto it = begin();
		for(auto ite=end(); it!=ite; ++it) 
			if(it->get_address() == address) break;
		return it;
	}

	template<typename P, typename... ARGS>
	bool LinkManager<P>::add(const id_type address, const ARGS... args) { //should add have index access?
		if( get_index().contains(address) ) {
			if( contains(address) ) return false;
			links.push_back( link_type(address, args...) );
			get_index().elem(address).links.push_back( link_type(links.back(), ID()) );
			return true;
		} else return false;
	}

	template<typename P, typename... ARGS>
	bool LinkManager<P>::clone(const self_type& other) { //how does clone know how to copy links?
	//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
		if( other.is_managed_by(get_index()) ) { 
			clear();
			id_type currentID;
			for(const link_type& x : other) {
				currentID = x.get_address();
				if(currentID != ID()) add(currentID, x.get_value());
			}
			return true;
		} else return false;
	}

	template<typename P, typename... ARGS>
	void remove(const id_type address, complement_type& other) {
		auto iter = find(address);
		if(iter != end()) remove(iter, other);
	}

	template<typename P, typename... ARGS>
	void LinkManager<P>::remove(const iterator iter, complement_type& other) {
		auto address = iter->get_address();
		links.erase(iter);
		other.clean_up(ID());
	}


} //namespace ben

#endif

