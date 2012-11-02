#ifndef BenoitNode_h
#define BenoitNode_h

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
	
	template<typename V>
	class UndirectedNode : public Singleton { 
	private:
		typedef UndirectedNode self_type;
		typedef Singleton base_type;
		
		struct PairedLink; //should not be exposed
	
	public:
		typedef Graph<UndirectedNode> index_type;
		typedef V value_type;
		typedef typename std::vector<PairedLink>::iterator iterator; //need custom iterator
		
	private:
		std::vector<PairedLink> links;
		
		void add(const id_type address, const PairedLink& x); //not implemented
		bool clone_links(const self_type& other); //not implemented
		bool clean_up(const id_type address); //not implemented
	
	public:
		iterator find(const id_type address); //not implemented
	
		bool add(const id_type address, const value_type value); //not implemented
		void remove(const iterator iter) { links.erase(iter); }
		void remove(const id_type address) { remove(find(address)); }
		void clear() { links.clear(); }
		
		size_t size() const { return links.size(); }
		
		iterator begin() { return links.begin(); }
		iterator end() { return links.end(); }
		
	}; //class UndirectedNode
	
	
	template<typename V>
	struct UndirectedNode<V>::PairedLink {
		shared_ptr<value_type> ptr; 
		id_type otherID;
	}; //struct PairedLink
	
	template<typename V>
	struct UndirectedNode<V[]>::PairedLink {
		shared_ptr<value_type[]> ptr; //use templates to specialize for array types
		id_type otherID;
	}; //struct PairedLink, array specialization
} 

#endif

