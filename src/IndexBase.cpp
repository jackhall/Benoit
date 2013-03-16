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

#include "IndexBase.h"
#include "Singleton.h"

namespace ben {
	
	bool IndexBase::update_singleton(Singleton* ptr) {
	//updates the tracking for the indicated Singleton
	//returns false if no Singleton with this ID is currently being tracked, true otherwise
		auto iter = index.find(ptr->ID());
		if(iter != index.end()) { 
			iter->second = ptr;
			return true;
		} else return false;
	}
	
	bool IndexBase::check(const id_type address, const Singleton* local_ptr) const {
	//verifies correct tracking of Singleton
		auto iter = index.find(address);
		if(iter != index.end()) return iter->second == local_ptr;
		else return false;
	} 
	
	bool IndexBase::add(Singleton& x) {
	//begins tracking x, removing it from its existing Index if necessary
	//returns false if this Index is already tracking a Singleton with x's ID, true otherwise
		if( index.insert(std::make_pair(x.ID(), &x)).second ) {
			if(!x.is_managed()) x.update_index(this);
			else {
				if(!x.is_managed_by(*this)) {
					x.index->remove(x.ID());
					x.update_index(this);
				}
			}	 
			return true; 
		} return false;
	}
	
	bool IndexBase::remove(const id_type address) {
	//stops tracking Singleton with ID=address, leaving it with a nullptr where the Singleton
	//was pointing to this Index
	//returns false it this Index is not tracking a Singleton with ID=address, true otherwise
		auto iter = index.find(address);
		if( iter != index.end() ) { 
			if(iter->second->is_managed_by(*this)) iter->second->update_index(nullptr); 
			index.erase(iter);
			return true;
		} else return false;
	}
	
	bool IndexBase::merge_into(std::shared_ptr<IndexBase> other_ptr) {
	//transfers management of all Singletons to other, first checking for redundancy
	//returns false if any Singletons had redundant IDs in other (reassign ID?), true else
	//either transfers all Singletons or none
		if(this == other_ptr.get()) return false; //redundant, but clear
		for(auto x : index) if( other_ptr->contains(x.first) ) return false;
		
		auto it = index.begin(), ite = index.end();
		while(it != ite) {
			//does not call add or remove!
			other_ptr->index.insert( std::make_pair(it->first, it->second) );
			it->second->update_index(other_ptr);
			index.erase(it++);
		}
	
		return true;
	}
	
} //namespace ben
