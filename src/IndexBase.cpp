#ifndef IndexBase_cpp
#define IndexBase_cpp

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
//#include "Singleton.h"

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
	
	bool IndexBase::add(Singleton* ptr) {
	//begins tracking referent of ptr
	//returns false if this Index is already tracking a Singleton with ptr's ID, true otherwise
	//only called by Singleton, internally
		auto insert_status = index.insert(std::make_pair(ptr->ID(), ptr));
		if( insert_status.second ) {
			bool delegate_status = perform_add(ptr); 
			if(!delegate_status) index.erase(insert_status.first);
			return delegate_status;
		} return false;
	}
	
	bool IndexBase::remove(const id_type address) {
	//stops tracking Singleton with ID=address, 
	//returns false if this Index is not tracking a Singleton with ID=address, true otherwise
	//only called by Singleton, internally
		auto iter = index.find(address);
		if( iter != index.end() ) {
			bool status = perform_remove(iter->second); 
			if(status) index.erase(iter);
			return status;
		} else return false;
	}

	IndexBase::~IndexBase() { //should never be called while any Singletons are still managed
		//but for safety's sake...
		for(auto x : index) x.second->update_index(std::shared_ptr<self_type>()); 
	}

} //namespace ben

#endif

