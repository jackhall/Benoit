#ifndef BenoitLink_h
#define BenoitLink_h

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011-2012  Jack Hall

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

#include <memory>

namespace ben {

	template<typename V, typename S, bool B>
	class Port {
	protected:
		std::shared_ptr< Link<V,S,B> > link_ptr;
		Port(Link<V,S,B>* ptr) : link_ptr(ptr) {}
		
	public:
		Port() = delete;
		Port(const Port&& rhs) : link_ptr( std::move(rhs) ) {}
		Port& operator=(const Port&& rhs) { 
			//check for sameness would be redundant because Port
			//assignment is only called by InPort or OutPort assignment
			link_ptr = std::move( rhs.link_ptr );
			return *this;
		}
		virtual ~Port() = default;
		
		V& get_value() const { return link_ptr->value; }
		void set_value(const V& v) const { link_ptr->value = v; }
		bool ready() const { return link_ptr->ready(); }
	};

	template<typename V, typename S, bool B>
	struct InPort : public Port<V,S,B> {
		unsigned int sourceID;
	
		InPort(Link<V,S,B>* ptr, unsigned int nSource) : Port(ptr), sourceID(nSource) {}
		InPort(const InPort&& rhs) : Port( std::move(rhs) ), sourceID(rhs.sourceID) {}
		InPort& operator=(const InPort&& rhs) {
			if(&rhs != this) {
				Port::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		
		unsigned int source() const { return source; }
		S pull() const { return link_ptr->pull(); }
	};
	
	template<typename V, typename S, bool B>
	struct OutPort : public Port<V,S,B> {
		unsigned int target;
	
		OutPort(Link<V,S,B>* ptr, unsigned int nTarget) : Port(ptr), target(nTarget) {}
		OutPort(const OutPort&& rhs) : Port( std::move(rhs) ), target(rhs.target) {}
		OutPort& operator=(const OutPort&& rhs) {
			if(&rhs != this) {
				Port::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		
		unsigned int target() const { return link_ptr->target; }
		void push(const S& signal) { link_ptr->push(signal); }
	};

} //namespace ben

