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
		
		bool update_singleton(Singleton* ptr) {
			auto iter = index.find(ptr->ID());
			if(iter != index.end()) { 
				iter->second = ptr;
				return true;
			} else return false;
		}
		
		void update_all() const;
			{ std::for_each(begin(), end(), [](singleton_type& x) { x.update_index(this); }); }
		
		bool check(const id_type address, const Singleton* local_ptr) const
			{ return index.find(address)->second == local_ptr; }
			
		virtual bool add(Singleton& x);
		virtual bool remove(const id_type address);
		virtual void clear();
	}; //class IndexBase	

} //namespace ben

#endif
