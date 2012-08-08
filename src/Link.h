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

#include <thread>
#include <atomic>

namespace ben {
	
	/*
		Links are the edges of a distributed directed graph structure. Each Link has an origin Node from which it conveys 
		data, and a target Node to which it conveys data. Links are owned by their target Node. A Link owns its own 
		pointer as it is stored in the origin Node. 
		
		The template parameters are shared with the owning Node. W is the type of the weight (for Links) or bias (for Nodes).
		S is the type of message passed between Nodes. The buffer is a statically allocated array of this type. The extra
		space (there are two) may be wasted, but static allocation allows for faster access than dynamic allocation. 
		
		The origin and target Nodes are stored by their ID rather than by pointer. This avoids hanging pointers if a Node 
		object gets moved around or copied. It does mean that the Link needs to have a copy of the Index pointer, though. 
		
		Links are not designed to be visible to the user of Benoit. They should be accessed via the Node::input_port or
		Node::output_port classes, which are a cross between iterators and stream objects. 
		
		Each Link will have a mutex member when multithreading is implemented. 
	*/
	template<typename S> 
	struct Signal {
		std::atomic<bool> ready; //needs to be accessed with a memory order policy
		std::atomic<S> data;
	
		Signal() : ready(false), data() {} //needs signal_type to be default-constructible
		Signal(const Signal&& rhs) : ready(rhs.ready), 
					     data( std::move(rhs.data) ) {}
		Signal& operator=(const Signal&& rhs) {
			if(this != &rhs) {
				ready = rhs.ready;
				data = std::move( rhs.data );
			}
		}
		
		void reset() { ready=false; data=S(); }
	};
	
	
	template<typename N> //make links atomic?
	struct AsyncLink {
	public:
		typedef typename N::value_type 	value_type;
		typedef typename N::signal_type signal_type;
	
		Signal<signal_type> front; //make this atomic? maybe make Links atomic at the Port level?
		value_type value;
		
		BaseLink() : value(), front() {} //needs signal_type to be default-constructible
		explicit BaseLink(const value_type& v) : value(v), front() {}
		BaseLink(const BaseLink& rhs) = delete; //no reason to have this
		BaseLink& operator=(const BaseLink& rhs) = delete; //would leave hanging pointers
		BaseLink& operator=(BaseLink&& rhs) = delete;
		~BaseLink() = default;
		
		bool ready() const { return front.ready; }
		void flush() { front.reset(); }
		//this method probably requires locking for thread safety
		void push(const signal_type& signal) { front = Signal<signal_type>{true, signal}; }
		signal_type pull() const { 
			front.ready = false;
			return front.data;
		}
	}; //class Link
	
	
	template<typename N>
	struct SyncLink : public AsyncLink<N> {
		Signal<signal_type> back;
		
		//following needs value_type to be default-constructible
		SyncLink(const value_type& v=value_type()) : AsyncLink(v), back() {}
		
		//need thread safety but preferably without locking
		//look up atomic operations 
		//	- these need to be built-in types 
		//	- use on Signal::ready?
		void flush() { front.reset(); back.reset(); }
		void push(const signal_type& signal) { 
			front = std::move( back );
			back = Signal<signal_type>{true, signal}; 
		}
	}
	
} //namespace ben

#endif
