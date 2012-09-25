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

namespace ben {
	
	template<typename I>
	class Singleton {
	public:
		typedef Singleton 	self_type;
		typedef I 		index_type;
		typedef unsigned int 	id_type;
	private:
		static std::atomic<id_type> IDCOUNT;  
		inline static id_type get_new_ID() { return IDCOUNT.fetch_add(1); }
	
	protected:
		id_type uniqueID;
		index_type* index;
		//std::mutex?
		
		void update_index(index_type* ptr) { index = ptr; }
		
	public:
		Singleton(const id_type nID=get_new_ID())
			: uniqueID(nID), index(nullptr) {}
		Singleton(index_type& x, const id_type nID=get_new_ID()) 
			: uniqueID(nID), index(&x) { if( !x.add(*this) ) throw; } //define a custom exception?

		//copyable?
		Singleton(self_type&& rhs) 
			: uniqueID(rhs.uniqueID), index(rhs.index) { 
			if( !update_singleton(this) ) throw; //define a custom exception?
			rhs.index = nullptr;
		}
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				uniqueID = rhs.uniqueID;
				if( !update_singleton(this) ) throw; //define a custom exception?
				rhs.index = nullptr;
			} 
			return *this;
		}
		virtual ~Singleton() { if(managed()) index->remove(uniqueID); }
		
		virtual bool operator<(const self_type& rhs) { return uniqueID < rhs.uniqueID; }
		bool managed() const { return index != nullptr; }
		const index_type& get_index() const { 
			if(index != nullptr) return *index;
			else throw; //define a custom exception for this? 
		}
		id_type ID() const { return uniqueID; }
		//resetID method? not for now
	}; //class Singleton
	
} //namespace ben

#endif
