#ifndef BenoitLink_h
#define BenoitLink_h

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
#include <type_traits>
#include <initializer_list>

namespace ben {
	
	/*
		Link is base class that does not touch message-passing behavior
		except for a protected typedef for the Frame struct. Instead, 
		Link handles value storage and access (atomic). Since it lacks
		push(), pull() and is_ready() methods, it doesn't not have enough
		traits to be wrapped by a conventional Port, even if it could be
		constructed (it can't).  
		
		For interoperability with the rest of Benoit, every Link type should 
		provide:
			value_type and signal_type typedefs
			get_value() and set_value() methods
			pull(), push() and is_ready() methods
			default construction and construction from a value_type reference
			
	*/		
	
	template<typename S> 
	struct Frame {
	/*
		POD Frame hopefully allows more compiler optimizations.
		This version of Frame is an aggregate, so the compiler will generate move and copy ctors!
	*/
		bool ready;
		S data;
	}; //struct Frame
	
	
	template<typename S>
	class LinkBase {
	/*
		A general link base class, including appropriate and convenient typedefs.
	*/
	public:
		typedef S signal_type;
	private:
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 
	protected:
		typedef Frame<S> frame_type;
	};
	
	
	template<typename S, unsigned short B> class Link;
	
	template<typename S>
	class Link<S,1> {
	/*
		A link with a one-element buffer.
	*/	
	private:
		typedef Frame<S> frame_type;
		
		std::atomic<frame_type> data;
		//std::atomic<int> test_atomic;
	
	public:
		typedef S signal_type;
		Link() noexcept : data({false, signal_type()}) {} 
		~Link() noexcept = default;
		
		void flush() { data.store(frame_type{false, signal_type()}); }
		bool is_ready() const { return data.load().ready; }
		bool push(const signal_type& signal) { //returns true if an unread signal is overwritten
			auto temp = data.exchange(frame_type{true, signal});
			return temp.ready;
		}
		
		signal_type pull() { 
			auto temp = data.exchange(frame_type{false, signal_type()});
			return temp.data; 
		}
	}; //class Link (length 1 specialization)
	
	
	template<typename S, unsigned short B>
	class Link {
	public:
		typedef S signal_type;
	private:
		typedef Frame<S> frame_type;
		
		Link<S,1> next;
		std::array<frame_type, B-1> buffer;
		unsigned short index;
		
		void reset_buffer() { 
			for(frame_type& x : buffer)
		       		{ x = frame_type{false, signal_type()}; }
		}
		
	public:
		Link() noexcept : next(), buffer(), index(0) { reset_buffer(); } 
		~Link() noexcept = default;
		
		void flush() { 
			next.flush();
			reset_buffer();
		} 
		
		bool is_ready() const { return next.is_ready(); }
		
		bool push(const signal_type& signal) {
			auto temp = buffer[index];
			buffer[index++] = frame_type{true, signal};
			if(index == B-1) index = 0;
			if(temp.ready) return next.push(temp.data);
			else return false;
		}
		
		signal_type pull() { return next.pull(); }
	}; //class Link (length n specialization)
	
	
	template<typename S>
	class Link<S,2> : public LinkBase<S> {
	/*
		A link with a two-element buffer.
	*/	
	public:
		typedef S signal_type;
	private:
		typedef Frame<S> frame_type;
	
		Link<S,1> next;
		frame_type buffer;
	
	public:
		Link() noexcept : next(), buffer{false, signal_type()} {} 
		~Link() noexcept = default;
		
		void flush() { 
			next.flush();
			buffer = frame_type{false, signal_type()}; 
		}
		bool is_ready() const { return next.is_ready(); }
		bool push(const signal_type& signal) { //returns true if an unread signal is overwritten
			auto temp = buffer;
			buffer = frame_type{true, signal};
			if(temp.ready) return next.push(temp.data);
			else return false;
		}
		
		signal_type pull() { return next.pull(); }
	}; //class Link (length 2 specialization)
	
} //namespace ben

#endif
