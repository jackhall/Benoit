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
	
	template<typename V, typename S>
	class Link<V,S,1> {
	/*
		Use 
	*/
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
		bool push(const signal_type& signal) { 
			front.store(signal, std::memory_order_release); 
			return true;
		}
		signal_type pull() { return front.load(std::memory_order_consume); }
	};
	
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
			//write signal to back and read previous value simultaneously
			//move previous value of back to front
			//flag link as ready to read and return true
			auto temp = back.exchange(signal, std::memory_order_acq_rel);
			front.store(temp, std::memory_order_release);
			ready.store(!is_new, std::memory_order_release); 
			is_new = false; //overhead to compensate for init period
			return true;
		}
		signal_type pull() { 
			if( ready.exchange(false, std::memory_order_acq_rel) ) 
				return front.load(std::memory_order_consume);
			else return signal_type();
		}
	};
	
} //namespace ben
