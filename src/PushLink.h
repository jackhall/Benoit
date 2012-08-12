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

namespace ben {
	
	/*
		Rewrite overview!
	*/	
	
	template<typename V, typename S, unsigned short B> 
	class PushLink {
	/*
		Use 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		//for convenience/readability
		typedef std::memory_order_consume consume;
		typedef std::memory_order_acquire acquire;
		typedef std::memory_order_release release;
		typedef std::memory_order_acq_rel acq_rel;
		
		std::atomic<value_type> value;
		std::array<signal_type, B> buffer;
		unsigned short index;
		std::atomic<bool> ready;
		std::mutex link_mutex;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs signal_type to be default-constructible
		explicit PushLink(const value_type& v) 
			: value(v), buffer(), ready(false), read_index(0), write_index(0) {}
		PushLink(const PushLink& rhs) = delete; //no reason to have this
		PushLink& operator=(const PushLink& rhs) = delete; //would leave hanging pointers
		PushLink& operator=(PushLink&& rhs) = delete;
		~PushLink() = default;
		
		value_type get_value() const { return value.load(consume); }
		void set_value(const value_type& v) { value.store(v, release); }
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
	class PushLink<V,S,0> { //use this for non-message-passing links later?
		PushLink() = delete; //prevent instantiation
	};
	
	
	template<typename V, typename S>
	class PushLink<V,S,1> {
	/*
		Use 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		std::atomic<value_type> value;
		std::atomic<bool> ready;
		std::atomic<signal_type> front;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) : value(v), front() {}
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			ready.store(false, std::memory_order_release);
			front.store(signal_type(), std::memory_order_release); 
			ready.store(false, std::memory_order_release); //to be safe
		}
		bool is_ready() const { return ready.load(std::memory_consume); }
		bool push(const signal_type& signal) { 
			front.store(signal, std::memory_order_release); 
			ready.store(true, std::memory_order_release);
			return true;
		}
		signal_type pull() { 
			ready.store(false, std::memory_order_acq_rel);
			return front.load(std::memory_order_consume); 
		}
	};
	
	
	template<typename V, typename S>
	class PushLink<V,S,2> {
	/*
		Use 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		//for convenience/readability
		typedef std::memory_order_consume consume;
		typedef std::memory_order_acquire acquire;
		typedef std::memory_order_release release;
		typedef std::memory_order_acq_rel acq_rel;
		
		enum class State {
			full_unread,
			full_read,
			half_full_unread,
			half_full_read,
			empty
		};
		std::atomic<value_type> value;
		std::atomic<State> state;
		std::atomic<signal_type> front, back;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) : value(v), state(empty), front(), back() {}
		
		value_type get_value() const { return value.load(consume); }
		void set_value(const value_type& v) { value.store(v, release); }
		void flush() { 
			state.store(empty, release);
			front.store(signal_type(), release); 
			back.store(signal_type(), release); 
		}
		bool is_ready() const { 
			return state.load(consume) == full_unread; 
		}
		bool push(const signal_type& signal) { 
			auto temp_state = state.load(acquire);
			switch(temp_state) { 
				case full_unread:
					front.load(consume);
					state.store(full_read, release);
					break;
				case half_full_read:
					back.store(signal, release);
					state.store(half_full_unread, release);
					break;
				case full_read:
					auto temp_item = back.exchange(signal, acq_rel);
					front.store(temp_item, release);
					state.store(full_unread, release);
					break;
				case half_full_unread: //tricky, need a mutex?
					state.store(full_unread, release);
					back.store(signal, release);
					break;
				default: //empty
					front.store(signal, release);
					back.store(signal, release);
					state.store(half_full_unread, release);
			}
			return true;
		}
		signal_type pull() { 
			signal_type result();
			auto temp_state = state.load(acquire);
			switch(temp_state) {
				case full_read:
				case full_unread:
					result = front.load(consume);
					state.store(full_read, release);
					break;
				case half_full_read:
				case half_full_unread:
					result = back.load(consume);
					state.store(half_full_read, release);
					break;
				default:
			}
			return result; 
		}
	};
	
} //namespace ben
