#ifndef BenoitIndexBase_h
#define BenoitIndexBase_h

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

#include <unordered_map>
#include <memory>
#include "Commons.h"

namespace ben {
	
	class Singleton;

/* An IndexBase, like a Singleton, is not meant to be instantiated. Unlike a Singleton, only the Index
 * class template should inherit from it. IndexBase encapsulates a map to match IDs to Singleton pointers,
 * like a directory. Like all of the high-level objects in Benoit, IndexBase has identity semantics and 
 * cannot be copied. Inheriting from Commons provides readers/writer locking.
 */
	class IndexBase : public wayne::Commons {
	public: 
		typedef unsigned int id_type;
		friend class Singleton;
	
		bool manages(const id_type address) const { return index.count(address) == 1; }
		size_t size() const { return index.size(); }
		bool check(const id_type address, const Singleton* local_ptr) const {
		//verifies correct tracking of Singleton
			auto iter = index.find(address);
			if(iter != index.end()) return iter->second == local_ptr;
			else return false;
		}

	private:
		typedef IndexBase self_type;

		//these functions perform custom operations on derived data structure
		//for context, see the exposed add, remove, and merge_into methods
		//return true if successful, roll back and return false if not
		virtual bool perform_add(Singleton* ptr) = 0; //make these members of Index instead?
		virtual void perform_remove(Singleton* ptr) = 0;

		bool update_singleton(Singleton* ptr);

	protected:
		IndexBase() = default;
		IndexBase(const self_type& rhs) = delete;
		IndexBase(self_type&& rhs) = delete;
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs) = delete;
		virtual ~IndexBase();
 
		typedef std::unordered_map<id_type, Singleton*> map_type;
		mutable map_type index;
		
		bool add(Singleton* ptr); 
		void remove(const id_type address);
	}; //class IndexBase	

} //namespace ben

#endif

