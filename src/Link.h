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
#include <type_traits>

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
	template<typename T>
	struct GenericFrame {
	/*
		GenericFrame is for any signals that are not POD or moveable.
	*/
		bool ready;
		T data;
		
		GenericFrame() noexcept : ready(false), data() {} //needs signal_type to be default-constructible
		GenericFrame(bool bReady, const T& x) : ready(bReady), data(x) {}
	}; //struct GenericFrame
	
	
	template<typename T>
	struct PODFrame {
	/*
		PODFrame specializes frames to hopefully allow more compiler optimizations.
	*/
		bool ready;
		T data;
	}; //struct PODFrame
	
	
	template<typename T>
	struct MoveableFrame {
	/*
		MoveableFrame is a moveable helper struct for use with Link types. It combines
		a signal and a boolean denoting whether the signal has been read. It 
		is meant to be treated as an atomic. 
	*/
		bool ready; 
		T data;

		MoveableFrame() noexcept : ready(false), data() {} //needs signal_type to be default-constructible
		MoveableFrame(bool bReady, const T& x) : ready(bReady), data(x) {}
		MoveableFrame(const MoveableFrame&& rhs) noexcept : ready(rhs.ready), data( std::move(rhs.data) ) {}
		MoveableFrame& operator=(MoveableFrame&& rhs) {
			if(this != &rhs) {
				ready = rhs.ready;
				data = std::move( rhs.data );
			}
		}
		~MoveableFrame() noexcept = default;
	}; //struct MoveableFrame
	
	
	template<typename V, typename S>
	class Link {
	/*
		A general link, including appropriate and convenient typedefs 
		and a value field. The value field is accessed atomically by
		getters and setters. 
	*/
	public:
		typedef V value_type;
		typedef S signal_type;
	protected:
		typedef MoveableFrame<signal_type> frame_type;
		
		std::atomic<value_type> value;
		
		Link() : Link(value_type()) {} //needs value_type to be default-constructible
		explicit Link(const value_type& v) : value(v) {}
		virtual ~Link() = default;
	
	public:
		value_type get_value() const { return value.load(std::memory_order_consume); }
		void set_value(const value_type& v) { value.store(v, std::memory_order_release); }
	}; //class Link
	
} //namespace ben

#endif
