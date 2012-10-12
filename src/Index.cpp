#ifndef BenoitIndex_cpp
#define BenoitIndex_cpp

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

#include "Index.h"
#include "Singleton.h"

namespace ben {

	Index::Index(Index&& rhs) : index( std::move(rhs.index) ) { update_all(); }
	
	Index& Index::operator=(Index&& rhs) {
		if(this != &rhs) {
			clear();
			index = std::move(rhs.index);
			update_all();
		} return *this;
	}
	
	bool Index::update_singleton(pointer x) {
		auto iter = index.find(x->ID());
		if(iter != index.end()) { 
			iter->second = x;
			return true;
		} else return false;
	}
	
	void Index::update_all() const
		{ std::for_each(begin(), end(), [](reference x) { x.update_index(this); }); }
	
	bool Index::add(reference x) { 
		if( index.insert(std::make_pair(x.ID(), &x))->second ) {
			x.update_index(this); return true;
		} return false;
	}
	
	bool Index::remove(const id_type address) { 
		auto iter = index.find(address);
		if( iter != index.end() ) { 
			if(iter->second->managed_by(*this)) iter->second->update_index(nullptr); 
			index.erase(iter);
			return true;
		} else return false;
	}	
	
	void Index::clear() { 
		std::for_each(begin(), end(), [](reference x) { x.update_index(nullptr); }); 
		index.clear(); 
	}
	
	bool Index::move_to(Index& other, const id_type address) {
		auto iter = index.find(address);
		if(iter != index.end()) {
			if( other.add(*(iter->second)) ) {
				index.erase(iter);
				return true;
			} else return false; //redundant ID in destination
		} else return false; //no element with that ID here
	}
	
	bool Index::merge_into(Index& other) {
		//returns true if all singletons were transferred
		//returns false if any had redundant IDs in other (reassign ID?)
		for(auto it=index.begin(), auto ite=index.end(); it!=ite; ++it) 
			if( other.add(*(it->second)) ) index.erase(it);
			
		return index.empty();
	}
	
} //namespace ben

#endif
