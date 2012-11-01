#ifndef BenoitPushLink_h
#define BenoitPushLink_h

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

#include <atomic>
#include <mutex>
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
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		typedef Link<V,S> base_type;
		typedef PushLink self_type;
		typedef typename base_type::frame_type frame_type;
		
		std::array<signal_type, B> buffer;
		unsigned short read_index, write_index;
		std::atomic<bool> ready;
		std::mutex link_mutex;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs signal_type to be default-constructible
		explicit PushLink(const value_type& v) 
			: base_type(v), buffer(), ready(false), read_index(0), write_index(0), link_mutex() {}
		PushLink(const self_type& rhs) = delete; //no reason to have this
		PushLink& operator=(const self_type& rhs) = delete; //would leave hanging pointers
		PushLink& operator=(self_type&& rhs) = delete;
		~PushLink() noexcept = default;
		
		using base_type::get_value;
		using base_type::set_value;
		void flush() { 
			std::lock_guard<std::mutex> lock(link_mutex);
			ready.store(false, std::memory_order_release);
			read_index = write_index = 0;
			for(signal_type& item : buffer)
				item = signal_type();
		}
		bool is_ready() const { return ready.load(std::memory_order_acquire); }
		bool push(const signal_type& signal) { 
			std::lock_guard<std::mutex> lock(link_mutex);
			buffer[write_index] = signal;
			if(write_index==read_index && is_ready()) 
				if(++read_index >= B) read_index = 0;
			if(++write_index >= B) write_index = 0;
			ready.store(true, std::memory_order_release);
			return true;
		} 
		signal_type pull() { 
			std::lock_guard<std::mutex> lock(link_mutex);
			auto temp = std::move(buffer[read_index]);
			if(ready.exchange(false, std::memory_order_acq_rel)) 
				if(++read_index >= B) read_index = 0;
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

	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		typedef Link<V,S> base_type;
		typedef PushLink self_type;
		typedef typename base_type::frame_type frame_type;

		std::atomic<bool> ready;
		std::atomic<signal_type> front;
		
	public:
		PushLink() noexcept : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) noexcept : base_type(v), ready(false), front() {}
		~PushLink() noexcept = default;
		
		using base_type::get_value;
		using base_type::set_value;
		void flush() { 
			ready.store(false, std::memory_order_release);
			front.store(signal_type(), std::memory_order_release); 
		}
		bool is_ready() const { return ready.load(std::memory_order_consume); }
		bool push(const signal_type& signal) { 
			front.store(signal, std::memory_order_release); 
			ready.store(true, std::memory_order_release);
			return true;
		}
		signal_type pull() { 
			ready.store(false, std::memory_order_release);
			return front.load(std::memory_order_consume); 
		}
	};
	
} //namespace ben

#endif

