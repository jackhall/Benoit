#ifndef BenoitSyncNode_h
#define BenoitSyncNode_h

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
	class SyncNode {
	private:
		typedef unsigned int id_type;	
		
		static id_type IDCOUNT; 
		inline static id_type get_new_ID() { return IDCOUNT++; }
	
		id_type nodeID;
		std::vector< InPort<V,S,true> > inputs;
		std::vector< OutPort<V,S,true> > outputs;
		Index<V,W>* index; 
		
	public:
		
	}; //SyncNode

} //namespace ben
