#ifndef BenoitPort_h
#define BenoitPort_h

/*
    Benoit: a flexible framework for distributed graphs and spaces
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
	
	/*
		Wraps and manages a link. Port cannot be instantiated, but each
		InPort and Output should be instantiated as a pair with a port of
		the complementary type. 
		
		For interoperability with the rest of Benoit, every Port type should 
		provide:
			link_type, value_type, signal_type, and id_type typedefs
			get_value, set_value and is_ready() methods
			construction from a link_type pointer and an integer
			move constructor and assignment operator
			input port:
				source() and pull() methods
			output port:
				target() and push() methods
			operator< to sort by ID
			
		The default implementations of the Port classes provide simple pass-throughs
		for Link method calls. More complex implementations could be written to store
		extra data, check validity of pull operations or deal with custom Link classes
		in a special way. 
	*/
	
	template<typename L, typename I=unsigned int>
	class Port {
	public:
		typedef L 			link_type;
		typedef typename L::value_type 	value_type;
		typedef typename L::signal_type signal_type;
		typedef I 			id_type;
	
	protected:
		std::shared_ptr<link_type> link_ptr;
		Port(link_type* ptr) : link_ptr(ptr) {}
		
		Port() = delete;
		Port(const Port& rhs) : link_ptr(rhs.link_ptr) {}
		Port(Port&& rhs) : link_ptr( std::move(rhs.link_ptr) ) {}
		Port& operator=(const Port& rhs) { link_ptr = rhs.link_ptr; return *this; }
		Port& operator=(Port&& rhs) { 
			//check for sameness would be redundant because Port
			//assignment is only called by InPort or OutPort assignment
			link_ptr = std::move( rhs.link_ptr );
			return *this;
		}
		virtual ~Port() = default;
	
	public:
		inline value_type get_value() const { return link_ptr->get_value(); }
		inline void set_value(const value_type& v) const { link_ptr->set_value(v); }
		inline bool is_ready() const { return link_ptr->is_ready(); }
	}; //class Port
	

	template<typename L> class OutPort;

	template<typename L>
	class InPort : public Port<L> {
	private:
		typedef Port<L> base_type;
		typedef InPort self_type;
		using base_type::link_ptr;
		
	public:
		typedef typename base_type::id_type id_type;
		typedef typename L::value_type value_type;
		typedef typename L::signal_type signal_type;
		typedef OutPort<L> complement_type;
		typedef L link_type;
	
		id_type sourceID;
	
		InPort(id_type nSource, value_type x = value_type()) 
			: base_type(new link_type(x)), sourceID(nSource) {}
		InPort(const complement_type& other, id_type nSource) : base_type(other), sourceID(nSource) {}
		InPort(const self_type& rhs) : base_type(rhs), sourceID(rhs.sourceID) {} //does Node need this?
		InPort(self_type&& rhs) : base_type( std::move(rhs) ), sourceID(rhs.sourceID) {}
		InPort& operator=(const self_type& rhs) {
			if(this != &rhs) {
				base_type::operator=(rhs);
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		InPort& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		//bool operator<(const self_type& rhs) const { return sourceID < rhs.sourceID; }
		
		inline id_type source() const { return sourceID; }
		inline signal_type pull() const { return link_ptr->pull(); }
	}; //struct InPort
	
	
	template<typename L>
	class OutPort : public Port<L> {
	private:
		typedef Port<L> base_type;
		typedef OutPort self_type;
		using base_type::link_ptr;
		
	public:
		typedef typename base_type::id_type id_type;
		typedef typename L::value_type value_type;
		typedef typename L::signal_type signal_type;
		typedef InPort<L> complement_type;
		typedef L link_type;
		
		id_type targetID;
	
		OutPort(id_type nTarget, value_type x = value_type()) 
			: base_type(new link_type(x)), targetID(nTarget) {}
		OutPort(const complement_type& other, id_type nTarget) : base_type(other), targetID(nTarget) {}
		OutPort(const self_type& rhs) : base_type(rhs), targetID(rhs.targetID) {} //does Node need this?
		OutPort(self_type&& rhs) : base_type( std::move(rhs) ), targetID(rhs.targetID) {}
		OutPort& operator=(const self_type& rhs) {
			if(this != &rhs) {
				base_type::operator=(rhs);
				targetID = rhs.targetID;
			}
			return *this;
		}
		OutPort& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				targetID = rhs.targetID;
			}
			return *this;
		}
		//bool operator<(const self_type& rhs) const { return targetID < rhs.targetID; }
		
		inline id_type target() const { return targetID; }
		inline bool push(const signal_type& signal) { return link_ptr->push(signal); } //take another look at const requirements
	}; //struct OutPort

} //namespace ben

#endif

