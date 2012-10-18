#ifndef BenoitIndexBase_h
#define BenoitIndexBase_h

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

#include <map>
#include "Commons.h"

namespace ben {
	
	class Singleton;

	class IndexBase : public wayne::Commons {
	/*
		IndexBase keeps track of Singleton pointers by the IDs of those Singletons. Most
		of its members are protected here and then hidden by Index<>. IndexBase is not meant
		to be instantiated except as a base class of Index<>. 
		
		Some of the methods (check, contains) are implemented here because their behavior and 
		interface would not conceivably change for more specialized Index-Singleton abstractions. 
		Others (add, remove, clear) are left open for redefinition, but provide default 
		implementations. In most cases, these default implementations should be called by their
		replacements, because the Singleton-tracking behavior is meant to be encapsulated by 
		IndexBase. 
		
		As a global data structure that may be accessed in multiple threads, IndexBase inherits
		from wayne::Commons to provide readers/writer locking. 
	*/
	public: 
		typedef unsigned int id_type;
		friend class Singleton;
	
		bool contains(const id_type address) const { return index.count(address) == 1; }
	
	private:
		typedef IndexBase self_type;
	
	protected:
		IndexBase()=default;
		IndexBase(const self_type& rhs) = delete;
		IndexBase(self_type&& rhs) : index( std::move(rhs.index) ) { update_all(); }
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				clear();
				index = std::move(rhs.index);
				update_all();
			} return *this;
		}
		virtual ~IndexBase() { clear(); }
	
		std::map<id_type, Singleton*> index;
		
		bool update_singleton(Singleton* ptr); 
		void update_all();
		
		bool check(const id_type address, const Singleton* local_ptr) const 
		//verifies correct tracking of Singleton
			{ return index.find(address)->second == local_ptr; } 
			
		virtual bool add(Singleton& x); 
		virtual bool remove(const id_type address);
		virtual void clear();
	}; //class IndexBase	

} //namespace ben

#endif

