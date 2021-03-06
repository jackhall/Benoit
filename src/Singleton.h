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

#include "IndexBase.h"
#include <memory>
#include <atomic>

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
	template<typename S> class Index; //is the forward declaration necessary now that merge is fully templated?
	
	template<typename T> 
	bool merge(std::shared_ptr<T> one, std::shared_ptr<T> two);

	class Singleton {
	public:
		typedef unsigned int id_type;
		id_type ID() const { return uniqueID; }
		bool is_managed() const { return static_cast<bool>(index_ptr); }
		void leave_index() { 
			if(index_ptr) { 
				perform_leave();
				index_ptr->remove(uniqueID); 
				index_ptr.reset();
			}
		}

	private:
		typedef Singleton self_type;
		typedef IndexBase index_type; 
		static std::atomic<id_type> IDCOUNT;  
		static id_type get_new_ID() { return IDCOUNT.fetch_add(1); }

		template<typename T>
		friend bool merge(std::shared_ptr<T>, std::shared_ptr<T>); 
		friend index_type; //only for safety in IndexBase destructor

		std::shared_ptr<index_type> index_ptr;
		id_type uniqueID;
		//std::mutex? maybe later
		
		void update_index(const std::shared_ptr<index_type>& ptr) { index_ptr = ptr; }
		virtual void perform_leave() = 0;
	
	protected:
		Singleton(const id_type id=get_new_ID())
			: uniqueID(id), index_ptr() {}
		Singleton(std::shared_ptr<index_type> ptr, const id_type id=get_new_ID()) //passing ptr by ref isn't thread-safe
			: uniqueID(id), index_ptr(ptr) { 
			while( !ptr->add(this) ) uniqueID = get_new_ID(); 
		} 
		Singleton(const self_type& rhs) = delete;
		Singleton(self_type&& rhs) : uniqueID(rhs.uniqueID), index_ptr(rhs.index_ptr) { 
			if( index_ptr && !(index_ptr->update_singleton(this)) ) throw; //define a custom exception?
			rhs.index_ptr.reset();
		}
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs);

		virtual ~Singleton() { if(index_ptr) index_ptr->remove(uniqueID); }
	
		bool join_index(std::shared_ptr<index_type> ptr);

		std::shared_ptr<index_type> get_index() const { return index_ptr; }
	}; //class Singleton
	
	std::atomic<typename Singleton::id_type> Singleton::IDCOUNT(1000);

	Singleton& Singleton::operator=(self_type&& rhs) {
		if(this != &rhs) {
			leave_index();
			uniqueID = rhs.uniqueID;
			index_ptr = std::move( rhs.index_ptr ); //haven't checked rhs.is_managed() yet
			if( index_ptr && !(index_ptr->update_singleton(this)) ) throw; //define a custom exception?
		} 
		return *this;
	}

	bool Singleton::join_index(std::shared_ptr<index_type> ptr) {
		if(ptr != index_ptr) {
			auto originalID = ID();
			while( ptr->manages(uniqueID) ) uniqueID = get_new_ID(); 
			bool status = ptr->add(this);
			if(status) {
				//since ID has changed, can't delegate to leave_index()
				if(index_ptr) index_ptr->remove(originalID); 
				index_ptr = ptr;
			} else uniqueID = originalID;
			return status;
		} else return true;
	}
	
} //namespace ben

#include "IndexBase.cpp"

#endif
