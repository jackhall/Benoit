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
	
	void update_all() const { 
		std::for_each(index.begin(), index.end(), [](std::pair<id_type, Singleton*>& x) { 
			x->second->update_index(this); 
		}); 
	}
	
	bool IndexBase::add(Singleton& x) {
		if( index.insert(std::make_pair(x.ID(), &x))->second ) {
			x.update_index(this); return true; 
		} return false;
	}
	
	bool IndexBase::remove(const id_type address) {
		auto iter = index.find(address);
		if( iter != index.end() ) { 
			if(iter->second->managed_by(*this)) iter->second->update_index(nullptr); 
			index.erase(iter);
			return true;
		} else return false;
	}
	
	void IndexBase::clear() {
		std::for_each(index.begin(), index.end(), [](std::pair<id_type, Singleton*>& x) { 
			x->second->update_index(nullptr); 
		}); 
		index.clear(); 
	}
	
} //namespace ben
