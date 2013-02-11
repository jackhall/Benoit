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
		typedef typename std::vector<link_type>::iterator iterator; 
		
	private:
		std::vector<link_type> links;
		
		void clean_up(const id_type address); 
	
	public:
		iterator find(const id_type address); //not implemented
	
		bool add(const id_type address, const value_type value); //not implemented
		bool clone_links(const self_type& other); //not implemented
		void remove(const iterator iter);
		void remove(const id_type address) { remove(find(address)); }
		void clear();
		
		size_t size() const { return links.size(); }

		bool contains(const id_type address) const { return links.end() != find(address); }
		
		self_type& walk(const iterator iter) const { return get_index().elem(iter->get_address()); }
		iterator begin() { return links.begin(); }
		iterator end() { return links.end(); }
	}; //class UndirectedNode

	
	template<typename V>
	void UndirectedNode<V>::clean_up(const id_type address) {
		auto iter = find(address);
		if(iter != end()) links.erase(iter);
	}

	template<typename V>
	typename UndirectedNode<V>::iterator UndirectedNode<V>::find(const id_type address) {
		auto it = begin();
		for(auto ite=end(); it!=ite; ++it) 
			if(it->get_address() == address) break;
		return it;
	}

	template<typename V>
	bool UndirectedNode<V>::add(const typename UndirectedNode<V>::id_type address) {
		
	}

	template<typename V>
	bool UndirectedNode<V>::clone_links(const self_type& other) {

	}

	template<typename V>
	void UndirectedNode<V>::remove(const iterator iter) {

	}

	template<typename V>
	void UndirectedNode<V>::clear() {

	}

} //namespace ben

#endif

