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
 * 
 * Any replacement must have:
 * 	id_type typedef
 * 	complement_type typedef
 * 	construction_types typedef
 * 	get_address() method
 * 	clone() method
 * 	Constructor(id_type, construction_types...)
 * 	Constructor(complement_type, id_type)
 * 	copy construction/assignment (for the STL)
 */
	template<typename B, typename I=unsigned int>
	class Port : buffer_traits<B> {
	//this is just code shared by InPort and OutPort, can't be instantiated by itself
	public:
		typedef B 			buffer_type;
		typedef typename B::signal_type signal_type;
		typedef I 			id_type;
		typedef ConstructionTypes<>	construction_types; //tells LinkManager how to construct a Port
	
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
		bool is_ghost() const { return buffer_ptr.use_count() < 2; } //necessary but not sufficient for ghost :(
	}; //class Port

	template<typename B> class InPort;
	template<typename B> class OutPort; //forward declaration to resolve circular typedefs

	//untested
	template<typename B> bool operator==(const InPort<B>& lhs, const InPort<B>& rhs);
	template<typename B> bool operator!=(const InPort<B>& lhs, const InPort<B>& rhs);
	template<typename B> bool operator==(const OutPort<B>& lhs, const OutPort<B>& rhs);
	template<typename B> bool operator!=(const OutPort<B>& lhs, const OutPort<B>& rhs);

	template<typename B>
	class InPort : public Port<B> {
/*This version allows pulling but not pushing, enforcing the directedness of messages
 */
	private:
		typedef Port<B> base_type;
		typedef InPort self_type;
		using base_type::buffer_ptr;
		
		typename base_type::id_type sourceID;

		friend bool operator==<B>(const self_type& lhs, const self_type& rhs);
		friend bool operator!=<B>(const self_type& lhs, const self_type& rhs);

	public:
		//carrying these typedefs through inheritance is automatic only for non-template classes
		typedef typename base_type::construction_types construction_types;
		typedef typename base_type::id_type id_type;
		typedef B buffer_type;
		typedef typename buffer_type::signal_type signal_type;
		typedef OutPort<buffer_type> complement_type;
	
		InPort(id_type nSource) : base_type(new buffer_type()), sourceID(nSource) {} //new link, new shared_ptr
		InPort(const complement_type& other, id_type nSource) : base_type(other), sourceID(nSource) {} //matching link, old shared_ptr
		InPort(const self_type& rhs) : base_type(rhs), sourceID(rhs.sourceID) {} //necessary for stl internals
		InPort(self_type&& rhs) : base_type( std::move(rhs) ), sourceID(rhs.sourceID) {}
		InPort& operator=(const self_type& rhs) {//increases use_count
			if(this != &rhs) {
				base_type::operator=(rhs);
				sourceID = rhs.sourceID;
			}
			return *this;
		}
		InPort& operator=(self_type&& rhs) {//preserves use_count
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				sourceID = rhs.sourceID;
			}
			return *this;
		}
	
		self_type clone(const id_type address) const { 
			//how to get a copy with a new shared_ptr
			//can't use sourceID because then links-to-self could not be cloned properly
			return self_type(address); 
		}
	
		id_type get_address() const { return sourceID; }
		bool pull(signal_type& signal) const { return buffer_ptr->pull(signal); }
	}; //struct InPort
	
	
	template<typename B>
	class OutPort : public Port<B> {
/*This version allows pushing but not pulling, enforcing the directedness of messages
 */
	private:
		typedef Port<B> base_type;
		typedef OutPort self_type;
		using base_type::buffer_ptr;
		
		typename base_type::id_type targetID;

		friend bool operator==<B>(const self_type& lhs, const self_type& rhs);
		friend bool operator!=<B>(const self_type& lhs, const self_type& rhs);

	public:
		typedef typename base_type::construction_types construction_types;
		typedef typename base_type::id_type id_type;
		typedef typename B::signal_type signal_type;
		typedef InPort<B> complement_type;
		typedef B buffer_type;
	
		OutPort(id_type nTarget) : base_type(new buffer_type()), targetID(nTarget) {} //new link, new shared_ptr
		OutPort(const complement_type& other, id_type nTarget) : base_type(other), targetID(nTarget) {} //matches existing complement
		OutPort(const self_type& rhs) : base_type(rhs), targetID(rhs.targetID) {}
		OutPort(self_type&& rhs) : base_type( std::move(rhs) ), targetID(rhs.targetID) {}
		OutPort& operator=(const self_type& rhs) {//increases shared_ptr::use_count
			if(this != &rhs) {
				base_type::operator=(rhs);
				targetID = rhs.targetID;
			}
			return *this;
		}
		OutPort& operator=(self_type&& rhs) { //preserves shared_ptr::use_count
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				targetID = rhs.targetID;
			}
			return *this;
		}
	
		self_type clone(const id_type address) const { 
			//how to get a copy with a new shared_ptr
			//can't use targetID because then links-to-self could not be cloned properly
			return self_type(address); 
		} 
	
		id_type get_address() const { return targetID; }
		bool push(const signal_type& signal) { return buffer_ptr->push(signal); } //take another look at const requirements
	}; //struct OutPort

	template<typename B>
	bool operator==(const InPort<B>& lhs, const InPort<B>& rhs) { return lhs.buffer_ptr == rhs.buffer_ptr; }
	template<typename B>
	bool operator!=(const InPort<B>& lhs, const InPort<B>& rhs) { return !operator==(lhs, rhs); }	
	template<typename B>
	bool operator==(const OutPort<B>& lhs, const OutPort<B>& rhs) { return lhs.buffer_ptr == rhs.buffer_ptr; }
	template<typename B>
	bool operator!=(const OutPort<B>& lhs, const OutPort<B>& rhs) { return !operator==(lhs, rhs); }	

} //namespace ben

#endif

