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
/* Links are data buffers for the messages passed between Nodes. They're templated for both the
 * message type and buffer length. There are two partial specializations for buffer length B=1 
 * and B=2. 
 *
 * B=1 requires only one atomic element and no indexing. The other specializations each
 * have a B=1 Link as a member, and use it as a kind of staging area. Data can only be pulled from
 * there, and so only the output port gives access to the rest of the Link code. Thus data races 
 * in the rest of the buffer are avoided, and Link B=1 encapsulates all atomic operations. 
 *
 * B=2 is special because it also needs no indexing. Larger Links are implemented as circular buffers. 
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
	
	
	template<typename S, unsigned short B> class Link;

	
	template<typename S>
	class Link<S,1> {
	/*
		A link with a one-element buffer.
	*/	
	private:
		typedef Frame<S> frame_type;
		std::atomic<frame_type> data;
		
	public:
		typedef S signal_type;
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 

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
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 

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
	class Link<S,2> {
	/*
		A link with a two-element buffer.
	*/	
	public:
		typedef S signal_type;
	private:
		typedef Frame<S> frame_type;
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 

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
