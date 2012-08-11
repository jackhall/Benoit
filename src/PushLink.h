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
#include <climits>

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
		signal_type pull() { return front.load(std::memory_order_consume); }
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
		std::atomic<value_type> value;
		std::atomic<unsigned char> count;
		std::atomic<signal_type> front, back;
		
	public:
		PushLink() : PushLink(value_type()) {} //needs value_type to be default-constructible
		PushLink(const value_type& v) : value(v), count(0), front(), back() {}
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			count.store(0, std::memory_order_release);
			front.store(signal_type(), std::memory_order_release); 
			back.store(signal_type(), std::memory_order_release); 
			count.store(0, std::memory_order_release); //to be safe
		}
		bool is_ready() const { return count.load(std::memory_consume) > 1; }
		bool push(const signal_type& signal) { //needs play-by-play
			auto temp = back.exchange(signal, std::memory_order_acq_rel);
			front.store(temp, std::memory_order_release);
			if( count.fetch_add(1, std::memory_order_acq_rel) >= UCHAR_MAX-1 )
				count.store(2, std::memory_order_release);
			return true;
		}
		signal_type pull() { return front.load(std::memory_order_consume); }
	};
	
} //namespace ben
