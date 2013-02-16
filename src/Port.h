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
#include "LinkManager.h"

namespace ben {
	
/* Both the user and Node use port objects to handle Buffers. Ports serve two main 
 * purposes. The first is to abstract away Buffer handling from Nodes, making both
 * classes easier to maintain. The second is to enforce the directedness of a link
 * by only allowing outputs to push data and inputs to pull. 
 *
 * These default implementations of the Port classes provide simple pass-throughs 
 * for Buffer method calls. More complex implementations could be written to store
 * extra data, check validity of pull operations or deal with custom Buffer classes
 * in a special way. 
 */
	
	template<typename B, typename I=unsigned int>
	class Port {
	public:
		typedef B 			buffer_type;
		typedef typename B::signal_type signal_type;
		typedef I 			id_type;
		typedef ConstructionTypes<>	construction_types;
	
	protected:
		std::shared_ptr<buffer_type> buffer_ptr;//reference-counted smart pointer
		Port(buffer_type* ptr) : buffer_ptr(ptr) {}
		
		Port() = delete;
		Port(const Port& rhs) : buffer_ptr(rhs.buffer_ptr) {}
		Port(Port&& rhs) : buffer_ptr( std::move(rhs.buffer_ptr) ) {}
		Port& operator=(const Port& rhs) { buffer_ptr = rhs.buffer_ptr; return *this; }
		Port& operator=(Port&& rhs) { 
			//check for sameness would be redundant because Port
			//assignment is only called by InPort or OutPort assignment
			buffer_ptr = std::move( rhs.buffer_ptr );
			return *this;
		}
		virtual ~Port() = default;
	
	public:
		inline bool is_ready() const { return buffer_ptr->is_ready(); }
		inline bool is_ghost() const { return buffer_ptr.use_count() < 2; }
	}; //class Port
	

	template<typename B> class OutPort;

	template<typename B>
	class InPort : public Port<B> {
	private:
		typedef Port<B> base_type;
		typedef InPort self_type;
		using base_type::buffer_ptr;
		
	public:
		typedef typename base_type::construction_types construction_types;
		typedef typename base_type::id_type id_type;
		typedef typename B::signal_type signal_type;
		typedef OutPort<B> complement_type;
		typedef B buffer_type;
	
		id_type sourceID;
	
		InPort(id_type nSource) : base_type(new buffer_type()), sourceID(nSource) {}
		InPort(const complement_type& other, id_type nSource) : base_type(other), sourceID(nSource) {}
		InPort(const self_type& rhs) : base_type(rhs), sourceID(rhs.sourceID) {} //necessary for stl internals
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
	
		self_type clone() const { return self_type(sourceID); }
	
		inline id_type get_address() const { return sourceID; }
		inline signal_type pull() const { return buffer_ptr->pull(); }
	}; //struct InPort
	
	
	template<typename B>
	class OutPort : public Port<B> {
	private:
		typedef Port<B> base_type;
		typedef OutPort self_type;
		using base_type::buffer_ptr;
		
	public:
		typedef typename base_type::construction_types construction_types;
		typedef typename base_type::id_type id_type;
		typedef typename B::signal_type signal_type;
		typedef InPort<B> complement_type;
		typedef B buffer_type;
		
		id_type targetID;
	
		OutPort(id_type nTarget) : base_type(new buffer_type()), targetID(nTarget) {}
		OutPort(const complement_type& other, id_type nTarget) : base_type(other), targetID(nTarget) {}
		OutPort(const self_type& rhs) : base_type(rhs), targetID(rhs.targetID) {}
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
	
		self_type clone() const { return self_type(targetID); }
	
		inline id_type get_address() const { return targetID; }
		inline bool push(const signal_type& signal) { return buffer_ptr->push(signal); } //take another look at const requirements
	}; //struct OutPort

} //namespace ben

#endif

