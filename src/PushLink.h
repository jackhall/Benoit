#ifndef BenoitPushLink_h
#define BenoitPushLink_h

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

#include <atomic>
#include "Link.h"

namespace ben {
	
	/*
		PushLink models a fixed_size buffer that is causally oriented
		around push() calls. Pulling a signal marks it as read, but the 
		signal is still accessible until the next push. In fact, any push 
		to a full buffer will knock the front element off (even if it hasn't 
		been read). All operations are atomic or otherwise protected and 
		safe for use between two threads (producer-consumer relationship). 
		
		temporary note: PushLink<2> specialization is not yet thread-safe. 
	*/	
	
	template<typename V, typename S, unsigned short B> 
	class PushLink : public Link<V,S> {
	/*
		The general form of PullLink, implementing a circular buffer. The
		read and write operations use the same index that is only moved
		by push commands. Unfortunately, there was no way to get push-oriented
		semantics without a mutex. The data could be protected, but the read/
		unread state could not be defined for certain operations. Push and pull 
		operations are done in constant time. 
	*/
	private:
		typedef std::memory_order_acq_rel acq_rel; //for convenience/readability
		
		std::array<signal_type, B> buffer;
		unsigned short index;
		std::atomic<bool> ready;
		std::mutex link_mutex;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs signal_type to be default-constructible
		explicit PushLink(const value_type& v) 
			: Link(v), buffer(), ready(false), index(0), link_mutex() {}
		PushLink(const PushLink& rhs) = delete; //no reason to have this
		PushLink& operator=(const PushLink& rhs) = delete; //would leave hanging pointers
		PushLink& operator=(PushLink&& rhs) = delete;
		~PushLink() = default;
		
		void flush() { 
			ready.store(false, release);
			read_index.store(0, release); write_index = 0;
			for(std::atomic<signal_type>& item : buffer)
				item.store(signal_type(), release);
		}
		bool is_ready() const { return ready.load(std::memory_consume); }
		bool push(const signal_type& signal) { 
			link_mutex.lock();
			buffer[index] = signal;
			++index;
			if(index >= B) {
				index = 0;
				ready.store(true, release);
			}
			link_mutex.unlock();
			return true;
		} 
		signal_type pull() { 
			link_mutex.lock();
			temp = buffer[index];
			link_mutex.unlock();
			return temp;
		} 
	}; //class PushLink
	
	
	template<typename V, typename S>
	class PushLink<V,S,0> : public Link<V,S> { //use this for non-message-passing links later?
		PushLink() = delete; //prevent instantiation
	};
	
	
	template<typename V, typename S>
	class PushLink<V,S,1> : public Link<V,S> {
	/*
		This specialization saves memory and computation time since no
		states or logic are needed to handle a multi-element buffer. 
	*/
	private:
		std::atomic<bool> ready;
		std::atomic<signal_type> front;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) : Link(v), ready(false), front() {}
		
		void flush() { 
			ready.store(false, release);
			front.store(signal_type(), release); 
			ready.store(false, release); //to be safe
		}
		bool is_ready() const { return ready.load(consume); }
		bool push(const signal_type& signal) { 
			front.store(signal, release); 
			ready.store(true, release);
			return true;
		}
		signal_type pull() { 
			ready.store(false, release);
			return front.load(consume); 
		}
	};
	
	
	template<typename V, typename S>
	class PushLink<V,S,2> : public Link<V,S> {
	/*
		Not yet thread-safe.
	*/
	private:
		typedef std::memory_order_acq_rel acq_rel; //for convenience/readability
	
		enum class State {
			full_unread,
			full_read,
			half_full,
			empty
		};
		
		std::atomic<State> state;
		std::atomic<signal_type> front, back;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) : Link(v), state(empty), front(), back() {}
		
		void flush() { 
			state.store(empty, release);
			front.store(signal_type(), release); 
			back.store(signal_type(), release); 
		}
		bool is_ready() const { return state.load(consume) == full_unread; }
		bool push(const signal_type& signal) { 
			auto temp_state = state.load(acquire);
			switch(temp_state) { 
				case full_unread:
					front.load(consume);
					state.store(full_read, release);
					break;
				case full_read:
					auto temp_item = back.exchange(signal, acq_rel);
					front.store(temp_item, release);
					//if pull happens here, state will be incorrect
					state.store(full_unread, release);
					break;
				case half_full: 
					back.store(signal, release);
					state.store(full_unread, release);
					break;
				default: //empty
					front.store(signal, release);
					state.store(half_full, release);
			}
			return true;
		}
		signal_type pull() { 
			signal_type result();
			auto temp_state = state.load(acquire);
			switch(temp_state) {
				case full_read:
				case full_unread:
					//if push happens here, it will load the wrong signal
					result = front.load(consume);
					state.store(full_read, release);
					break;
				default:
			}
			return result; 
		}
	};
	
} //namespace ben
