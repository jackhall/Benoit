#ifndef BenoitGraph_h
#define BenoitGraph_h

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

#include <map>
#include <iostream>
#include "Index.h"

namespace ben {
/* Graph is the manager of a distributed directed graph consisting of the Nodes and Links that connect
 * them. As an Index, it does not own the Nodes (or it would not be a distributed structure). 
 *
 * Nodes may not be connected between Indicies. When an individual Node is moved, all Links must be cleared from it. 
 * Indicies may be swapped or merged, in which cases the Links between Nodes are preserved. 
 *
 * See Index and IndexBase for more information. Graph adds very little to these base classes. 
 */	
	template<typename N> 
	class Graph : public Index<N> {
	private:
		typedef Index<N> base_type;

		bool perform_add(Singleton* ptr) { return true; }
		void perform_remove(Singleton* ptr) {} //clean-up of links is performed by the Node calling remove
		bool perform_merge(base_type& other) { return true; } //no work necessary; links are left intact
	
	public:
		typedef N 	node_type;
		typedef N* 	pointer;
		typedef N&	reference;
		typedef size_t	size_type;
		typedef typename N::id_type id_type;
		typedef typename base_type::singleton_type singleton_type;
		//typedef typename base_type::iterator iterator; //necessary?

		Graph() = default;
		Graph(const Graph& rhs) = delete; //identity semantics
		Graph& operator=(const Graph& rhs) = delete; 
		Graph(Graph&& rhs) = delete;
		Graph& operator=(Graph&& rhs) = delete;
		~Graph() = default; 
	}; //class Graph

} //namespace ben

#endif

