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

namespace ben {
	/*
		Graph is the manager of a distributed directed graph consisting of the Nodes and Links that connect
		them. As an Index, it does not own the Nodes (or it would not be a distributed structure). 
		
		Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
		Indicies may be swapped or merged, in which cases the Links between Nodes are preserved.  
		
		See Index and IndexBase for more information. 
	*/
	
	template<typename N> 
	struct Graph : public Index<N> {
		typedef N 	node_type;
		typedef N* 	pointer;
		typedef N&	reference;
		typedef size_t	size_type;
		typedef typename N::id_type id_type;
		typedef Index<N> base_type;
		//typedef typename base_type::iterator iterator; //necessary?

		Graph()=default;
		Graph(const Graph& rhs) = delete;
		Graph(Graph&& rhs) : base_type( std::move(rhs) ) {}
		Graph& operator=(const Graph& rhs) = delete; 
		Graph& operator=(Graph&& rhs) { base_type::operator=( std::move(rhs) ); }
		virtual ~Graph() = default;
		
		virtual bool remove(const id_type address);
	}; //class Graph
	
	
	template<typename N>
	Graph<N>::~Graph() { for(node_type& x : *this) x.clear(); }
	
	template<typename N> 
	bool Graph<N>::remove(const id_type address) {
		auto iter = find(address);
		if(iter != end()) iter->clear();
		else return false;
		
		base_type::remove(address);
		return true;
	}
	
} //namespace ben

#endif

