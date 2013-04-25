#ifndef BenoitPoint_h
#define BenoitPoint_h

/*
	Eiffel: the simulation of a self-organizing network of trusses
	Copyright (C) 2012  John Wendell Hall

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
	
	The author may be reached at jackhall@utexas.edu.
*/

#include <array>
#include <functional>
#include <algorithm>

namespace ben {
	
	template<typename T, unsigned short N> 
	class CartesianPoint {
		typedef CartesianPoint self_type;
		std::array<T,N> data;
		constexpr unsigned short dimensions() { return N; }

	public:
		typedef T value_type;
		typedef std::array<T,N>::iterator iterator;
		typedef std::array<T,N>::const_iterator const_iterator;

		CartesianPoint() = default;
		template<typename... U>
		CartesianPoint(U... elements) : data{elements...} {}
		CartesianPoint(const self_type& rhs) = default;
		CartesianPoint& operator=(const self_type& rhs) = default;
		virtual ~CartesianPoint() = default;

		value_type& operator[](const unsigned short n) { return data[n]; }
		const value_type& operator[](const unsigned short n) const { return data[n]; }

		iterator begin() { return data.begin(); }
		const_iterator begin() const { return data.begin(); }
		iterator end() { return data.end(); }
		const_iterator end() const { return data.end(); }
	};


	template<typename T, unsigned short N> 
	CartesianPoint<T,N> for_both(const CartesianPoint<T,N> a, const CartesianPoint<T,N> b, 
				     std::function<T(T,T)> f) {
		CartesianPoint<T,N> c;
		auto i = a.begin(), j = b.begin();
		auto g = [&i, &j](T& x) { x = f(*i, *j); ++i; ++j; }
		std::for_each(c.begin(), c.end(), g);
		return c;
	}


	template<typename T, unsigned short N>
	T squared_distance(const CartesianPoint<T,N> a, const CartesianPoint<T,N> b) {
		//create a temporary point that is the difference between a & b
		auto c = for_both(c.begin(), c.end(), difference);

		//sum the elements of the temporary
		num_type d = 0;
		auto sum = [&d](num_type x){ d += x; }
		std::for_each(c.begin(), c.end(), sum);
		return d;
	}


	template<typename T, unsigned short N>
	class Point : public CartesianPoint<T,N>, public Singleton {
	private:
		typedef Point self_type;

	public:	
		typedef CartesianPoint<T,N> 		point_type;
		typedef typename point_type::value_type value_type;
		typedef typename Singleton::id_type 	id_type;

		Point() = default;
		explicit Point(std::shared_ptr<index_type> space_ptr) 
			: singleton_type(space_ptr), point_type() {}
		Point(std::shared_ptr<index_type> space_ptr, const id_type id)
			: singleton_type(space_ptr, id), point_type() {} 
		template<typename... U>
		Point(std::shared_ptr<index_type> space_ptr, U... coords) : Singleton(space_ptr), point_type(coords...) {}
		template<typename... U>
		Point(index_type& space, const id_type id, U... coords) : Singleton(space_ptr, id), point_type(coords...) {}
		Point(std::shared_ptr<index_type>, space_ptr, const point_type& alias) 
			: Singleton(space_ptr), point_type(alias) {}
		Point(std::shared_ptr<index_type> space_ptr, const id_type id, const point_type& alias)
			: singleton_type(space, id), point_type(alias) {}
		Point(const self_type& rhs) = delete;
		Point(self_type&& rhs) : Singleton(std::move(rhs)), point_type(rhs) {}
		self_type& operator=(self_type&& rhs) {
			if(this != &rhs) {
				Singleton::operator=(std::move(rhs));	
				point_type::operator=(rhs);
			}
			return *this;
		}
		~Point() = default;
		
		//void lock() { point_mutex.lock(); }
		//bool try_lock() { return point_mutex.try_lock(); }
		//void unlock() { point_mutex.unlock(); }
		
	}; //class Point
	
} //namespace ben

#endif

