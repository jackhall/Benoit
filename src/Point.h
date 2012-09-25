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
#include <atomic>
#include <initializer_list>
#include "Point.h" 

namespace ben {
	
	template<typename T, unsigned short N>
	class PointAlias : public wayne::Point<double,N> {
	public:
		typedef unsigned int id_type;
		typedef T coordinate_type;	
		typedef wayne::Point<double,N> base_type;
	
	private:
		template<unsigned short M=0, typename dummy=void> struct compareLT {
			static bool call(const PointAlias& one, const PointAlias& two) {
				if( one.template elem<M>() < two.template elem<M>() ) {
					return true;
				} else if( one.template elem<M>() > two.template elem<M>() ) {
					return false;
				} else return compareLT<M+1>::call(one, two);
			}
		};
		
		template<typename dummy> struct compareLT<N-1,dummy> {
			static bool call(const PointAlias& one, const PointAlias& two) {
				if( one.template elem<N-1>() < two.template elem<N-1>() ) {
					return true;
				} else if( one.template elem<N-1>() > two.template elem<N-1>() ) {
					return false;
				} else return one.pointID < two.pointID; //points are otherwise equal
			}
		};
	
	protected:
		id_type pointID;
		
	public:
		PointAlias() : base_type(), pointID(0) {}
		explicit PointAlias(const id_type nID) : base_type(), pointID(nID) {} //empty set
		PointAlias(std::initializer_list<coordinate_type> coords, const id_type nID)
			: base_type(coords), pointID(nID) {}
		PointAlias(const PointAlias& rhs) : base_type(rhs), pointID(rhs.pointID) {}
		PointAlias& operator=(const PointAlias& rhs) = default;
		virtual ~PointAlias() noexcept = default;
		
		bool operator<(const PointAlias& rhs) const {
			return compareLT<>::call(*this, rhs);
		}
	}; 
	
	
	template<typename T, unsigned short N>
	struct Point : public wayne::Point<T,N>, public Singleton<Space<Point>> {
	
		typedef wayne::Point<T,N> point_type;
		typedef Singleton<Space<Point>> singleton_type;
		typedef typename singleton_type::id_type id_type;
		typedef T coordinate_type;

		explicit Point(index_type& space) 
			: singleton_type(space), point_type() {}
		Point(index_type& space, const id_type id)
			: singleton_type(space, id), point_type() {} 
		Point(index_type& space, std::initializer_list<coordinate_type> coords)
			: singleton_type(space), point_type(coords) {}
		Point(index_type& space, std::initializer_list<coordinate_type> coords, const id_type id) 
			: singleton_type(space, id), point_type(coords) {}
		Point(const Point& rhs)
			: singleton_type(*rhs.index), point_type(rhs) {}
		Point(const Point& rhs, const id_type id)
			: singleton_type(*rhs.index, id), point_type(rhs) {}
		Point(index_type& space, const PointAlias& alias) 
			: singleton_type(space), point_type(alias) {}
		Point(index_type& space, const PointAlias& alias, const id_Type id) 
			: singleton_type(space, id), point_type(alias) {}
		Point& operator=(const Point& rhs) {
			if(this != &rhs) {
				id_type old_id = base_type::ID();
				base_type::operator=(rhs);
				if(space==nullptr) base_type::pointID = get_new_ID();
				else base_type::pointID = old_id; //keep old_id if it was valid before
				space->move_to(*rhs.space, this->ID());
			}
			return *this;
		}
		Point& operator=(const base_type& rhs) {
			if(this != &rhs) { 
				base_type::operator=(rhs);
			}
			return *this;
		}
		virtual ~Point() = default;
		
		//void lock() { point_mutex.lock(); }
		//bool try_lock() { return point_mutex.try_lock(); }
		//void unlock() { point_mutex.unlock(); }
		
	}; //class Point
	
} //namespace ben

#endif

