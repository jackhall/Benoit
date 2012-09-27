#ifndef BenoitGraph_h
#define BenoitGraph_h

/*
    Benoit: a flexible framework for distributed graphs
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

#include <map>
#include <iostream>
#include "Lock.h"

namespace ben {
	/*
		Graph is a manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. It does not own the Nodes (or it would not be a distributed structure), but it stores pointers to them 
		in an STL map, using the Node ID as the key value. 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		Each Graph will have both a read and a write mutex when multithreading is implemented.
	*/
	
	template<typename N> 
	class Graph : public Index<N> {
	public:
		typedef N 	node_type;
		typedef N* 	pointer;
		typedef N&	reference;
		typedef size_t	size_type;
		typedef typename N::id_type id_type;
		typedef Index<N> base_type;
		//typedef typename base_type::iterator iterator; //necessary?

		Graph()=default;
		Graph(const Graph& rhs) = delete;
		Graph(Graph&& rhs) : base_type(rhs) {}
		Graph& operator=(const Graph& rhs) = delete; 
		Graph& operator=(Graph&& rhs) { base_type::operator=(rhs); }
		virtual ~Graph() = default;
		
		virtual bool remove(const id_type address);  
		virtual bool move_to(Graph& other, const id_type address); //move individual Node
		virtual bool merge_into(Graph& other); 
	}; //class Graph
	
	template<typename N> 
	bool Graph<N>::remove(const id_type address) {
		auto iter = index.find(address);
		if( iter != index.end() ) { 
			iter->second->clear(); //this is the only difference from Index version
			if(iter->second->managed_by(*this)) iter->second->update_index(nullptr); 
			index.erase(iter);
			return true;
		} else return false;
	}
	
	template<typename N> 
	bool Graph<N>::move_to(Graph& other, const id_type address) {
		auto iter = index.find(address);
		if(iter != index.end()) {
			iter->second->clear(); //this is the only difference from Index version
			if( other.add(*(iter->second)) ) {
				index.erase(iter);
				return true;
			} else return false; //redundant ID in destination
		} else return false; //no element with that ID here
	}
	
	template<typename N> 
	bool Graph<N>::merge_into(Graph& other){
		for(auto it=index.begin(), auto ite=index.end(); it!=ite; ++it) 
			if( other.contains(it->first) ) return false;
		
		return base_type::merge_into(other);
	}
	
} //namespace ben

#endif
