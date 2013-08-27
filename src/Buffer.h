#ifndef BenoitBuffer_h
#define BenoitBuffer_h

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
/* Buffers are for the messages passed between Nodes. They're templated for both the
 * message type and buffer length. There are two partial specializations for buffer length B=1 
 * and B=2. 
 *
 * B=1 requires only one atomic element and no indexing. The other specializations each
 * have a B=1 Buffer as a member, and use it as a kind of staging area. Data can only be pulled from
 * there, and so only the output port gives access to the rest of the Buffer code. Thus data races 
 * in the rest of the buffer are avoided, and Buffer B=1 encapsulates all atomic operations. 
 *
 * B=2 is special because it also needs no indexing. Larger Buffers are implemented as circular buffers. 
 *
 * Any replacement must match the public interface, which is the same for all specializations. 
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
	
	
	template<typename S, unsigned short B> class Buffer;

	
	template<typename S>
	class Buffer<S,1> {
	/*
		A link with a one-element buffer. All atomic operations needed for 
		message-passing are encapsulated in this partial specialization. The
		other specializations use this one recursively.
	*/	
	private:
		typedef Frame<S> frame_type;
		std::atomic<frame_type> data;
		
	public:
		typedef S signal_type;
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 
		//also copy-constructible and assignable?

		Buffer() noexcept : data({false, signal_type()}) {} 
		~Buffer() noexcept = default;
		
		bool push(const signal_type& signal) { //returns true if an unread signal is overwritten
			auto temp = data.exchange(frame_type{true, signal});
			return !temp.ready;
		}
		
		bool pull(signal_type& signal) { 
			//it may be faster and safer to return as a pair with a boolean
			//that way, the buffer can't be altered between calls to is_ready
			//and pull
			auto temp = data.exchange(frame_type{false, signal_type()}); //leave the buffer blank
			if(temp.ready) signal = temp.data; 
			return temp.ready;
		}
	}; //class Buffer (length 1 specialization)
	
	
	template<typename S, unsigned short B>
	class Buffer {
	public:
		typedef S signal_type;
		
	private:
		typedef Frame<S> frame_type;
		//this recursion encapsulates all atomic operations in Buffer<S,1>
		//it also enforces the static_assert used in Buffer<S,1>
		Buffer<S,1> next;
		std::array<frame_type, B-1> buffer;
		unsigned short index;
		
		void reset_buffer() { 
			for(frame_type& x : buffer)
		       		{ x = frame_type{false, signal_type()}; }
		}
		
	public:
		Buffer() noexcept : next(), buffer(), index(0) { reset_buffer(); } 
		~Buffer() noexcept = default;
		
		bool push(const signal_type& signal) { //returns true if unread data is overwritten
			auto temp = buffer[index];
			buffer[index++] = frame_type{true, signal};
			if(index == B-1) index = 0; //index needs to cycle (buffer is circular)
			if(temp.ready) return next.push(temp.data);
			else return true;
		}
		
		bool pull(signal_type& signal) { return next.pull(signal); }
	}; //class Buffer (length n specialization)
	
	
	template<typename S>
	class Buffer<S,2> {
	/*
		A link with a two-element buffer. This is specialized because no indicies 
		are needed, unlike the general case. 
	*/	
	public:
		typedef S signal_type;
	private:
		typedef Frame<S> frame_type;
	
		Buffer<S,1> next; //the recursion encapsulates all atomic operations in Buffer<S,1>
		frame_type buffer;
	
	public:
		Buffer() noexcept : next(), buffer{false, signal_type()} {} 
		~Buffer() noexcept = default;
		
		bool push(const signal_type& signal) { //returns true if an unread signal is overwritten
			auto temp = buffer;
			buffer = frame_type{true, signal};
			if(temp.ready) return next.push(temp.data);
			else return true; 
		}
		
		bool pull(signal_type& signal) { return next.pull(signal); }
	}; //class Buffer (length 2 specialization)
	
} //namespace ben

#endif
