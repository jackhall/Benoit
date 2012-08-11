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

#include <atomic>

namespace ben {
	
	/*
		Rewrite overview!
	*/
	template<typename V, typename S, unsigned short B> 
	class Link {
	/*
		Use 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		struct Frame {
			bool ready; 
			signal_type data;
	
			Frame() : ready(false), data() {} //needs signal_type to be default-constructible
			Frame(const Frame&& rhs) : ready(rhs.ready), 
						   data( std::move(rhs.data) ) {}
			Frame& operator=(const Frame&& rhs) {
				if(this != &rhs) {
					ready = rhs.ready;
					data = std::move( rhs.data );
				}
			}
		};
		
		//for convenience/readability
		typedef std::memory_order_consume consume;
		typedef std::memory_order_acquire acquire;
		typedef std::memory_order_release release;
		
		std::atomic<value_type> value;
		std::array<std::atomic<Frame>, B> buffer;
		unsigned short read_index, write_index;
		
	public:
		Link() : value() {} //needs signal_type to be default-constructible
		explicit Link(const value_type& v) : value(v), buffer() {}
		Link(const Link& rhs) = delete; //no reason to have this
		Link& operator=(const Link& rhs) = delete; //would leave hanging pointers
		Link& operator=(Link&& rhs) = delete;
		~Link() = default;
		
		value_type get_value() const { return value.load(consume); }
		void set_value(const value_type& v) { value.store(v, release); }
		void flush() { 
			for(std::atomic<Frame> item : buffer)
				item.store(Frame(), release);
		}
		bool push(const signal_type& signal) { 
			//checks item at write_index
			//if it hasn't been read, return false (buffer is full)
			//if it has been read, overwrite it, increment write_index & return true
			auto item_temp = buffer[write_index].load(acquire);
			if(!temp.ready) return false; //reading has overtaken writing
			else {
				buffer[write_index].store(Frame{true,signal}, release);
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
			signal_type result();
			auto temp = buffer[read_index].load(consume);
			if(temp.ready) {
				temp.ready = false;
				buffer[read_index].store(temp, release); 
				result = temp.data;
				if(read_index >= (B-1)) read_index = 0;
				else ++read_index;
			} 
			return result;
		} 
	}; //class Link
	
	template<typename V, typename S>
	class Link<V,S,2> {
	/*
		Use 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		std::atomic<value_type> value;
		std::atomic<bool> ready;
		std::atomic<signal_type> front, back;
		bool is_new;
		
	public:
		Link() : Link(value_type()) {} //needs value_type to be default-constructible
		Link(const value_type& v) : value(v), front(), back(), is_new(true) {}
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			front.store(Frame<signal_type>(), std::memory_order_release); 
			back.store(Frame<signal_type>(), std::memory_order_release); 
		}
		bool push(const signal_type& signal) { 
			//check if 
			//write signal to back and read previous value simultaneously
			//move previous value of back to front
			//flag link as ready to read and return true
			if(ready.load(std::memory_order_acquire)) return false;
			else {
			
			}
			/*auto temp = back.exchange(signal, std::memory_order_acq_rel);
			front.store(temp, std::memory_order_release);
			ready.store(!is_new, std::memory_order_release); 
			is_new = false; //overhead to compensate for init period
			return true;*/
		}
		signal_type pull() { 
			if( ready.exchange(false, std::memory_order_acq_rel) ) 
				return front.load(std::memory_order_consume);
			else return signal_type();
		}
	};
	
} //namespace ben

