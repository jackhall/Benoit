/*
	Benoit: distributed graph and space data structures
	Copyright (C) 2012  John Wendell Hall

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
	
	The author may be reached at jackhall@utexas.edu.
*/

#include "Index.cpp"
#include "gtest/gtest.h"

struct DerivedIndex : public Index {

};

struct DerivedSingleton : private Singleton {
	
	DerivedSingleton(const id_type id=get_new_ID()) : Singleton(id) {}
	DerivedSingleton(DerivedIndex& x, const id_type id=get_new_ID()) 
		: Singleton(x,
		: uniqueID(id), index(&x) { if( !x.add(*this) ) throw; } //define a custom exception?

	DerivedSingleton(DerivedSingleton&& rhs) 
		: uniqueID(rhs.uniqueID), index(rhs.index) { 
		if( !update_singleton(this) ) throw; //define a custom exception?
		rhs.index = nullptr;
	}
	DerivedSingleton& operator=(DerivedSingleton&& rhs) {
		if(this != &rhs) {
			if(index != nullptr) 
				if( !(index->remove(uniqueID)) ) throw;
			uniqueID = rhs.uniqueID;
			index = rhs.index;
			if( !update_singleton(this) ) throw; //define a custom exception?
			rhs.index = nullptr;
		} 
		return *this;
	}
	virtual ~DerivedSingleton() {}
	
	bool managed() const { return Singleton::managed(); }
	bool managed_by(const DerivedIndex& x) const { return &get_index() == &x; }
	const DerivedIndex& get_index() const { return static_cast<const DerivedIndex&>(Singleton::get_index()); }
	id_type ID() const { return Singleton::ID(); }

	void update_index(Index* ptr) { Singleton::update_index(ptr); }
	bool switch_index(Index* ptr) { Singleton::switch_index(ptr); }
	
};


