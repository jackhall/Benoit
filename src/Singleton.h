#ifndef BenoitSingleton_h
#define BenoitSingleton_h

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

#include "Index.h"
#include <memory>

namespace ben {
	
/* A singleton class has identity semantics and is linked to an Index to track like objects.
 * Singletons are not meant to be instantiated, but to act as a base class and interface for
 * parts of a distributed data structure, like the nodes of a graph. Singletons cannot be 
 * copied, but they can be moved. This ensures that their place in the global structure is not
 * inadvertently duplicated. A side effect is that Singletons can't be stored in STL containers.
 * 
 * Singletons are designed as a base class, and as such most of the interface is protected.
 * The reason for this is that the type of Index used should be specified for type safety, and
 * circularity prevents the use of an Index<> reference here. Child classes cannot violate 
 * encapsulation, but the protected interface is semantically complete. 
 */
	class Singleton {
	public:
		typedef typename IndexBase::id_type id_type;
	private:
		typedef Singleton 	self_type;
		typedef IndexBase 	index_type;
		static std::atomic<id_type> IDCOUNT;  
		static id_type get_new_ID() { return IDCOUNT.fetch_add(1); }
	
		std::shared_ptr<index_type> index_ptr;
		id_type uniqueID;
		//std::mutex? maybe later
		
		void update_index(const std::shared_ptr<index_type>& ptr) { index_ptr = ptr; }
	
	protected:
		friend class IndexBase; 
	
		Singleton(const id_type id=get_new_ID())
			: uniqueID(id), index_ptr() {}
		Singleton(std::shared_ptr<index_type> ptr, const id_type id=get_new_ID()) //passing ptr by ref isn't thread-safe
			: uniqueID(id), index_ptr(ptr) { if( !ptr->add(*this) ) throw; } //define a custom exception?
		Singleton(const self_type& rhs) = delete;
		Singleton(self_type&& rhs) 
			: uniqueID(rhs.uniqueID), index_ptr(rhs.index_ptr) { 
			if( is_managed() && !(index->update_singleton(this)) ) throw; //define a custom exception?
			rhs.index_ptr.reset();
		}
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				if(index_ptr) //checks if index_ptr manages is empty
					if( !(index->remove(uniqueID)) ) throw; //need to catch this?
				uniqueID = rhs.uniqueID;
				index_ptr = rhs.index_ptr; //haven't checked rhs.is_managed() yet
				if( is_managed() && !(index_ptr->update_singleton(this)) ) throw; //define a custom exception?
				rhs.index_ptr.reset(); 
			} 
			return *this;
		}
		virtual ~Singleton() { if(is_managed()) index_ptr->remove(uniqueID); }
	
		bool is_managed() const { return index_ptr; }
		bool is_managed_by(const index_type& x) const { return index_ptr.get() == &ptr; }
		id_type ID() const { return uniqueID; }
		//resetID method? not for now
		std::shared_ptr<index_type> get_index() const { return index_ptr; }
	}; //class Singleton
	
	std::atomic<typename Singleton::id_type> Singleton::IDCOUNT(1000);
	
} //namespace ben

#endif
