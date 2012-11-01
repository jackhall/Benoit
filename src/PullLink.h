#ifndef BenoitPullLink_h
#define BenoitPullLink_h

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
#include <array>
#include "Link.h"

namespace ben {
	
	/*
		PullLink models a fixed-size buffer that is causally oriented
		around pull() calls. Pulling a signal pops it from the buffer, so
		each pull will either return a new element or a blank element (if
		the buffer is empty). If the buffer is full, push() fails and 
		returns false. All operations are atomic and safe for use between
		two threads (producer-std::memory_order_consumer relationship). 
	*/
	
	template<typename V, typename S, unsigned short B> 
	class PullLink : public Link<V,S> {
	/*
		The general form of PullLink, implementing a circular buffer. The
		read and write operations (and the indicies for each) are isolated
		from each other. Each signal is atomized with a bool that indicates
		whether that signal has been read. Since this logical information is
		loaded simultaneously with the signal data, there can be no destructive 
		overlap problems between thread logics. Value semantics are inherited 
		from Link. Push and pull operations are done in constant time. 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		typedef Link<V,S> base_type;
		typedef PullLink self_type;
		typedef typename base_type::frame_type frame_type;
		
		std::array<std::atomic<frame_type>, B> buffer;
		unsigned short read_index, write_index;
		
	public:
		PullLink() noexcept : PullLink(value_type()) {} //needs signal_type to be default-constructible
		explicit PullLink(const value_type& v) noexcept
			: base_type(v), buffer(), read_index(0), write_index(0) {}
		PullLink(const self_type& rhs) = delete; //no reason to have this
		PullLink& operator=(const self_type& rhs) = delete; //would leave hanging pointers
		PullLink& operator=(self_type&& rhs) = delete;
		~PullLink() noexcept = default;
		
		using base_type::get_value;
		using base_type::set_value;
		void flush() { 
			for(std::atomic<frame_type>& item : buffer)
				item.store(frame_type(), std::memory_order_release);
			read_index = 0; write_index = 0;
		}
		bool is_ready() const { return buffer[read_index].load(std::memory_order_consume).ready; }
		bool push(const signal_type& signal) { 
			//checks item at write_index
			//if it hasn't been read, return false (buffer is full)
			//if it has been read, overwrite it, increment write_index & return true
			auto item_temp = buffer[write_index].load(std::memory_order_acquire);
			if(item_temp.ready) return false; //reading has overtaken writing
			else {
				buffer[write_index].store(frame_type{true, signal}, std::memory_order_release);
				if(write_index >= (B-1)) write_index = 0;
				else ++write_index;
				return true;
			}
		} 
		signal_type pull() { 
			//check item at read_index
			//if it hasn't been read, overwrite it with ready=false, data=data,
			//	increment read_index & return its signal
			//if it has been read, return a blank signal (buffer is empty)
			signal_type result;
			auto temp = buffer[read_index].load(std::memory_order_consume);
			if(temp.ready) {
				temp.ready = false;
				result = temp.data;
				buffer[read_index].store(std::move(temp), std::memory_order_release); 
				if(read_index >= (B-1)) read_index = 0;
				else ++read_index;
			} 
			return result;
		} 
	}; //class PullLink
	
	
	template<typename V, typename S>
	class PullLink<V,S,0> : public Link<V,S> { //use this for non-message-passing links later?
		PullLink() = delete; //prevent instantiation
	};
	
	
	template<typename V, typename S>
	class PullLink<V,S,1> : public Link<V,S> {
	/*
		This specialization saves memory and computation time since no
		states or logic are needed to handle a multi-element buffer. 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		typedef Link<V,S> base_type;
		typedef PullLink self_type;
		typedef typename base_type::frame_type frame_type;
		
		std::atomic<frame_type> front;
		
	public:
		PullLink() noexcept : PullLink(value_type()) {} //needs value_type to be default-constructible
		explicit PullLink(const value_type& v) noexcept : base_type(v), front() {}
		~PullLink() noexcept = default;
		
		using base_type::get_value;
		using base_type::set_value;
		void flush() { front.store(frame_type(), std::memory_order_release); }
		bool is_ready() const { return front.load(std::memory_order_consume).ready; }
		bool push(const signal_type& signal) { 
			if(!front.load(std::memory_order_acquire).ready) {
				front.store(frame_type{true, signal}, std::memory_order_release); 
				return true;
			} else return false;
		}
		signal_type pull() { 
			auto temp_item = front.load(std::memory_order_acquire);
			if(temp_item.ready) {
				temp_item.ready = false;
				front.store(std::move(temp_item), std::memory_order_release);
				return temp_item.data; 
			} else return signal_type();
		}
	};
	
	
	template<typename V, typename S>
	class PullLink<V,S,2> : Link<V,S> {
	/*
		This specialization uses marginally less storage and computation by
		using individual variables for the buffer slots and bools for the read
		and write indicies. 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		typedef Link<V,S> base_type;
		typedef PullLink self_type;
		typedef typename base_type::frame_type frame_type;
	
		bool read_index, write_index; 
		std::atomic<frame_type> zeroth, first; 
		
	public:
		PullLink() noexcept : PullLink(value_type()) {} //needs value_type to be default-constructible
		PullLink(const value_type& v) noexcept
			: base_type(v), zeroth(), first(), read_index(false), write_index(false) {}
		~PullLink() noexcept = default;
		
		using base_type::get_value;
		using base_type::set_value;
		void flush() { 
			zeroth.store(frame_type(), std::memory_order_release);
			first.store(frame_type(), std::memory_order_release); 
			read_index = write_index = false; 
		}
		bool is_ready() const { 
			if(read_index) 	return first.load(std::memory_order_consume).ready; 
			else 		return zeroth.load(std::memory_order_consume).ready;
		}
		bool push(const signal_type& signal) { 
			if(write_index) {
				auto temp_item = first.load(std::memory_order_acquire);
				if(!temp_item.ready) { 
					first.store(frame_type{true, signal}, std::memory_order_release);
					write_index = false;
					return true;
				} else return false;
			} else { 
				auto temp_item = zeroth.load(std::memory_order_acquire);
				if(!temp_item.ready) { 
					zeroth.store(frame_type{true, signal}, std::memory_order_release);
					write_index = true;
					return true;
				} else return false;
			}
		}
		signal_type pull() { 
			signal_type result;
			if(read_index) {
				auto temp_item = first.load(std::memory_order_acquire);
				if(temp_item.ready) {
					temp_item.ready = false;
					first.store(std::move(temp_item), std::memory_order_release);
					result = temp_item.data;
					read_index = false;
				}
			} else {
				auto temp_item = zeroth.load(std::memory_order_acquire);
				if(temp_item.ready) {
					temp_item.ready = false;
					zeroth.store(std::move(temp_item), std::memory_order_release);
					result = temp_item.data;
					read_index = true;
				}
			}
			return result;
		}
	};
	
} //namespace ben

#endif

