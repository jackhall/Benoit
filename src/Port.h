#ifndef BenoitPort_h
#define BenoitPort_h

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

	template<typename L>
	class Port {
	public:
		typedef L 			link_type;
		typedef typename L::value_type 	value_type;
		typedef typename L::signal_type signal_type;
		typedef typename L::id_type 	id_type;
	protected:
		std::shared_ptr<link_type> link_ptr;
		Port(link_type* ptr) : link_ptr(ptr) {}
		
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
		
		inline value_type& get_value() const { return link_ptr->get_value(); }
		inline void set_value(const value_type& v) const { link_ptr->set_value(v); }
		inline bool ready() const { return link_ptr->ready(); }
	};

	template<typename L>
	struct InPort : public Port<L> {
		id_type sourceID;
	
		InPort(link_type* ptr, id_type nSource) : Port(ptr), sourceID(nSource) {}
		InPort(const InPort&& rhs) : Port( std::move(rhs) ), sourceID(rhs.sourceID) {}
		InPort& operator=(const InPort&& rhs) {
			if(&rhs != this) {
				Port::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		bool operator<(const InPort& rhs) const { return sourceID < rhs.sourceID; }
		
		inline id_type source() const { return source; }
		inline S pull() const { return link_ptr->pull(); }
	};
	
	template<typename L>
	struct OutPort : public Port<L> {
		id_type targetID;
	
		OutPort(L* ptr, id_type nTarget) : Port(ptr), target(nTarget) {}
		OutPort(const OutPort&& rhs) : Port( std::move(rhs) ), target(rhs.target) {}
		OutPort& operator=(const OutPort&& rhs) {
			if(&rhs != this) {
				Port::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		bool operator<(const OutPort& rhs) const { return targetID < rhs.targetID; }
		
		inline id_type target() const { return link_ptr->target; }
		inline void push(const S& signal) { link_ptr->push(signal); }
	};

} //namespace ben

