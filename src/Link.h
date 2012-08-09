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
	struct Frame {
		bool ready; 
		S data;
	
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
	
	
	template<typename V, typename S, unsigned short B> 
	class Link {
	public:
		typedef V value_type;
		typedef S signal_type;
	private:	
		std::atomic<value_type> value;
		std::array<std::atomic<Frame<signal_type>>, B> buffer;
		std::atomic<unsigned short> read_index, write_index;
		void increment_read() {
			auto temp = read_index.load();
			if(temp >= B-1) read_index.store(0);
		}
		
	public:
		Link() : value() {} //needs signal_type to be default-constructible
		explicit Link(const value_type& v) : value(v), buffer() {}
		Link(const Link& rhs) = delete; //no reason to have this
		Link& operator=(const Link& rhs) = delete; //would leave hanging pointers
		Link& operator=(Link&& rhs) = delete;
		~Link() = default;
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			for(std::atomic<Frame<signal_type>> item : buffer)
				item.store(Frame<signal_type>(), std::memory_order_release);
		}
		void push(const signal_type& signal) { 
			auto temp = buffer[write_index].load(std::memory_order_acquire);
			if(temp.ready) { //reading has overtaken writing
				
				auto ridx_temp = read_index.fetch_add(1);
				if(ridx_temp >= B) read_index.fetch_sub(B); 
				
			} else {
				
			}
		} 
		signal_type pull() {  } 
	}; //class Link
	
	
	template<typename V, typename S>
	class Link<V,S,0> { //use this for non-message-passing links later?
		Link() = delete; //prevent instantiation
	};
	
	
	template<typename V, typename S>
	class Link<V,S,1> {
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		std::atomic<value_type> value;
		std::atomic<signal_type> front;
		
	public:
		Link() : Link(value_type()) {} //needs value_type to be default-constructible
		Link(const value_type& v) : value(v), front() {}
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			front.store(signal_type(), std::memory_order_release); 
		}
		void push(const signal_type& signal) { 
			front.store(signal, std::memory_order_release); 
		}
		signal_type pull() { return front.load(std::memory_order_consume); }
	};
	
	
	template<typename V, typename S>
	class Link<V,S,2> {
	public:
		typedef V value_type;
		typedef S signal_type;
	private:
		std::atomic<value_type> value;
		std::atomic<bool> ready;
		std::atomic<signal_type> front, back;
				
	public:
		Link() : Link(value_type()) {} //needs value_type to be default-constructible
		Link(const value_type& v) : value(v), front(), back() {}
		
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
		void flush() { 
			front.store(Frame<signal_type>(), std::memory_order_release); 
			back.store(Frame<signal_type>(), std::memory_order_release); 
		}
		void push(const signal_type& signal) { 
			auto temp = back.exchange(signal, std::memory_order_acq_release);
			front.store(temp, std::memory_order_release);
			ready.store(true);
		}
		signal_type pull() { 
			if(ready.exchange(false)) 
				return front.load(std::memory_order_consume);
			else return signal_type();
		}
	};
	
} //namespace ben

#endif
