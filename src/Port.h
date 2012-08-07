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

	template<typename V, typename S>
	class Port {
	protected:
		shared_ptr< Link<V,S> > link_ptr;
		Port(Link<V,S>* ptr) : link_ptr(ptr) {}
		
	public:
		Port() = delete;
		Port(const Port&& rhs) : link_ptr( std::move(rhs) ) {}
		
		bool synch() const { return link_ptr->synchronous; }
		void set_synch() { link_ptr->synchronous = true; }
		void reset_synch() { link_ptr->synchronous = false; }
		V& get_value() const { return link_ptr->value; }
		void set_value(const V& v) const { link_ptr->value = v; }
		bool ready() const { return link_ptr->ready; }
	};

	template<typename V, typename S>
	struct InPort : public Port<V,S> {
		InPort(Link<V,S>* ptr) : Port(ptr) {}
		InPort(const InPort&& rhs) : Port( std::move(rhs) ) {}
		
		unsigned int source() const { return link_ptr->source; }
		S pull() const { return link_ptr->pull(); }
	};
	
	template<typename V, typename S>
	struct OutPort : public Port<V,S> {
		OutPort(Link<V,S>* ptr) : Port(ptr) {}
		OutPort(const OutPort&& rhs) : Port( std::move(rhs) ) {}
	
		unsigned int target() const { return link_ptr->target; }
		void push(const S& signal) { link_ptr->push(signal); }
	};

} //namespace ben

