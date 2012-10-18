#ifndef BenoitSingleton_h
#define BenoitSingleton_h

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

namespace ben {
	/*
		Singletons are unique local objects that track and are tracked by an Index. Each
		Singleton should have a unique identifier, and default instantiations use static
		members to guarantee this. However, since coordination of multiple Singletons may
		be necessary, client code is allowed to specify an ID at construction or assignment. 
		These semantics are similar to const, but allow flexibility without breaking the
		abstraction. 
		
		Singletons are designed as a base class, and as such most of the interface is protected.
		The reason for this is that the type of Index used should be specified for type safety, and
		circularity prevents the use of an Index<> reference here. Child classes cannot violate 
		encapsulation, but the protected interface is semantically complete. 
	*/
	class Singleton {
	public:
		typedef typename IndexBase::id_type id_type;
	private:
		typedef Singleton 	self_type;
		typedef IndexBase 	index_type;
		static std::atomic<id_type> IDCOUNT;  
		static id_type get_new_ID() { return IDCOUNT.fetch_add(1); }
	
		id_type uniqueID;
		index_type* index;
		//std::mutex?
		
		void update_index(index_type* ptr) { index = ptr; }
	
	protected:
		friend class IndexBase; 
	
		bool switch_index(index_type* ptr) { //not meant to be exposed
			if( !(ptr->contains(uniqueID)) ) {
				if(index != nullptr) 
					if( !(index->remove(uniqueID)) ) throw; //index did not manage this
				index = ptr;
				index->add(*this);
				return true;
			} else return false;
		}
		
		Singleton(const id_type id=get_new_ID())
			: uniqueID(id), index(nullptr) {}
		Singleton(index_type& x, const id_type id=get_new_ID()) 
			: uniqueID(id), index(&x) { if( !x.add(*this) ) throw; } //define a custom exception?
		Singleton(const self_type& rhs) = delete;
		Singleton(self_type&& rhs) 
			: uniqueID(rhs.uniqueID), index(rhs.index) { 
			if( managed() && !(index->update_singleton(this)) ) throw; //define a custom exception?
			rhs.index = nullptr;
		}
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				if(index != nullptr) 
					if( !(index->remove(uniqueID)) ) throw;
				uniqueID = rhs.uniqueID;
				index = rhs.index;
				if( managed() && !(index->update_singleton(this)) ) throw; //define a custom exception?
				rhs.index = nullptr;
			} 
			return *this;
		}
		virtual ~Singleton() { if(managed()) index->remove(uniqueID); }
		
		bool managed() const { return index != nullptr; }
		bool managed_by(const index_type& x) const { return index == &x; }
		id_type ID() const { return uniqueID; }
		//resetID method? not for now
	}; //class Singleton
	
	std::atomic<typename Singleton::id_type> Singleton::IDCOUNT(1000);
	
} //namespace ben

#endif
