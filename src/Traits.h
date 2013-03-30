#ifndef BenoitTraits_h
#define BenoitTraits_h

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

#include <type_traits>
#include "Singleton.h"

namespace ben {
	
	//template<typename X>
	//class LessThanComparable { //test this before writing any more concept checks for member functions
	//	template <void (X::*)()> struct Empty {};
	//	template<class T> static constexpr bool Helper(Empty< &T::operator< >*) { return true; }
	//	template<class T> static constexpr bool Helper(...) { return false; } //what does this mean?
	//public:
	//	static constexpr bool result() { return Helper<X>(0); }
	//};

	//the struct carries around its template argument pack, like a variadic typedef
	template<typename... ARGS> struct ConstructionTypes {};

	template<typename T> 
	struct path_traits {
		typedef typename T::id_type id_type;
		typedef typename T::complement_type complement_type;
		typedef typename T::construction_types construction_types;

		template<typename... ARGS> struct test_construction { static constexpr bool value = false; };
		template<typename... ARGS> struct test_construction< ConstructionTypes<ARGS...> > {
			static constexpr bool value = std::is_constructible<T, const id_type, ARGS...>::value;
		};

		//template<id_type (T::*U)()> struct test_get_address { static constexpr bool value = false; };
		//template<id_type (T::*U)()> struct test_get_address<&T::get_address> { static constexpr bool value = true; };
		//static_assert(test_get_address<&T::get_address>::value, "Path objects need 'id_type get_address() const' member function");

		static_assert(test_construction<construction_types>::value,
				"Path objects need to be constructible from (const id_type, construction_types...)");
		static_assert(std::is_constructible<T, complement_type&, const id_type>::value,
				"Path objects need to be constructible from (complement_type&, const id_type)");
		static_assert(std::is_copy_constructible<T>::value, "Path objects need a copy constructor");
		static_assert(std::is_assignable<T, T>::value, "Path objects need an assignment operator");

		//id_type (*test_get_address)() const;

		//static_assert(std::is_same<decltype(&test_get_address), decltype(&T::get_address)>::value,
		//		"Path objects need 'id_type get_address() const' member function");
		//static_assert(std::is_same<T(const id_type) const, decltype(&T::clone)>::value,
		//		"Path objects need '[self_type] clone(const id_type) const' member function");
		//static_assert(std::is_member_function<decltype(&T::get_address)>::value, 
		//path_traits() { test_construction<construction_types> test; }
	};
	
	/*template<typename X>
	class IDTraits {
		static_assert(std::is_default_constructible<X>::value, 
				"An ID should be default-constructible");
		static_assert(std::is_copy_constructible<X>::value,
				"An ID should be copy-constructible");
		static_assert(std::is_assignable<X>::value,
				"An ID should be assignable");
    		static_assert(LessThanComparable<X>::result(),
    				"An ID should be less-than comparable");
    		//equality comparable
    		//incrementable (for Singleton::IDCOUNT)
	};
	
	template<typename X>
	class LinkValueTraits {
		static_assert(std::is_default_constructible<X>::value, 
				"A Link's value_type should be default-constructible");
		static_assert(std::is_copy_constructible<X>::value,
				"A Link's value_type should be copy-constructible");
		static_assert(std::is_assignable<X>::value,
				"A Link's value_type should be assignable");
	};
	
	template<typename X>
	class LinkSignalTraits {
		static_assert(std::is_default_constructible<X>::value, 
				"A Link's signal_type should be default-constructible");
		static_assert(std::is_copy_constructible<X>::value, 
				"A Link's signal_type should be copy-constructible");
		static_assert(std::is_assignable<X>::value,
				"A Link's signal_type should be assignable");
		//move constructible and move assignable?
	};
	
	template<typename X>
	class LinkTraits {
		typedef typename X::value_type value_type;
		typedef typename X::signal_type signal_type;
	
		//if Links are abstracted away by Ports, this next is unnecessary
		static_assert(std::is_constructible<X, value_type&>, 
				"It should be possible to construct a Link from an object of its value_type");
	};
	
	template<typename X>
	class PortTraits {
		typedef typename X::link_type link_type;
		typedef typename X::id_type id_type;
		typedef typename X::complement_type complement_type;
		
		static_assert(std::is_constructible<X, link_type*, id_type>::value,
				"A Port should be constructible from a Link pointer and an ID");
		static_assert(std::is_constructible<X, complement_type&, id_type>::value,
				"A Port should be constructible from a reference to its complement and an ID");
		
		//const value_type& get_value() const;
		//const void set_value(const value_type&) const;
		//bool is_ready() const;
		
		LinkTraits<link_type> link; //or some other way to test that link_type actually is a link
	};
	
	template<typename X>
	class InputPortTraits : public PortTraits<X> {
		//id_type source() const;
		//signal_type pull() const;
	};
	
	template<typename X>
	class OutputPortTraits : public PortTraits<X> {	
		//id_type target() const;
		//bool push();
	};

	template<typename X>
	class SingletonTraits {
		static_assert(std::is_base_of<Singleton, X>::value, 
				"A Singleton class used to instantiate Index<> should inherit from Singleton");
		static_assert(!std::is_copy_constructible<X>::value,
				"Singletons should not be copied");
		static_assert(!std::is_assignable<X>::value,
				"Singletons should not be assigned");
		//static_assert(std::is_move_constructible<X>::value, //not actually necessary, just preferred
		//		"A Singleton should be move-constructible");
		//static_assert(std::is_move_assignable<X>::value,
		//		"A Singletons should be move-assignable");
	};
	*/	
}

#endif
