#ifndef BenoitAsyncNode_h
#define BenoitAsyncNode_h

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

#include <vector>

namespace ben {

	template<typename V, typename S>
	class AsyncNode {
	public:
		typedef unsigned int id_type;
		typedef Link<V,S,false> link_type;
	private:
		
		static id_type IDCOUNT; 
		inline static id_type get_new_ID() { return IDCOUNT++; }
	
		id_type nodeID;
		std::vector< InPort<link_type> > inputs;
		std::vector< OutPort<link_type> > outputs;
		Index<link_type>* index; 
		
	public:
		static Index<link_type> INDEX;
	}; //AsyncNode

} //namespace ben
