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
	enum class SignalTrait {copyable, POD, moveable};
	
	
	template<typename S, SignalTrait T> 
	struct Frame {
	/*
		Generic Frame is for any signals that are not POD or moveable.
	*/
		bool ready;
		S data;
		 
		Frame() noexcept : ready(false), data() {} 
		Frame(bool bReady, const S& x) : ready(bReady), data(x) {}
	}; //struct Generic Frame
	
	
	template<typename S> 
	struct Frame<S, SignalTrait::POD> {
	/*
		POD Frame specializes frames to hopefully allow more compiler optimizations.
		
		This version of Frame is an aggregate, so the compiler will generate move and copy ctors!
	*/
		bool ready;
		S data;
	}; //struct POD Frame
	
	
	template<typename S> 
	struct Frame<S, SignalTrait::moveable> {
	/*
		Moveable Frame is a moveable helper struct for use with Link types. It combines
		a signal and a boolean denoting whether the signal has been read. It 
		is meant to be treated as an atomic. 
	*/
		bool ready; 
		S data;

		Frame() noexcept : ready(false), data() {} 
		Frame(bool bReady, const S& x) : ready(bReady), data(x) {}
		Frame(const Frame&& rhs) noexcept : ready(rhs.ready), data( std::move(rhs.data) ) {}
		Frame& operator=(Frame&& rhs) {
			if(this != &rhs) {
				ready = rhs.ready;
				data = std::move( rhs.data );
			}
		}
		~Frame() noexcept = default;
	}; //struct Moveable Frame
	
	
	template<typename S>
	class Link {
	/*
		A general link, including appropriate and convenient typedefs.
	*/
	public:
		typedef S signal_type;
	
	private:
		static_assert(std::is_default_constructible<signal_type>::value, 
			      "signals should be default-constructible"); 
	
		template<bool can_copy, bool is_pod, bool can_move>
		struct Trait_Helper { static constexpr SignalTrait value = SignalTrait::copyable; };
		
		template<bool can_move>
		struct Trait_Helper<true, true, can_move> { static constexpr SignalTrait value = SignalTrait::POD; };
		
		template<bool can_copy>
		struct Trait_Helper<can_copy, false, true> { static constexpr SignalTrait value = SignalTrait::moveable; };
		
		template<bool is_pod>
		struct Trait_Helper<false, is_pod, false> {}; //need some way to pass signals around
	
	public:
		static constexpr SignalTrait signal_trait 
			= Trait_Helper<std::is_copy_constructible<signal_type>::value
					&& std::is_copy_assignable<signal_type>::value,
					std::is_pod<signal_type>::value,
					std::is_move_constructible<signal_type>::value
					&& std::is_move_assignable<signal_type>::value>::value;
	
	protected:
		typedef Frame<S, signal_trait> frame_type;
		virtual ~Link() = default;
	}; //class Link
	
	template<typename S>
	constexpr SignalTrait Link<S>::signal_trait;
	
} //namespace ben

#endif
