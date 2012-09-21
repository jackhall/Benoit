#ifndef ContinuousPoint_h
#define ContinuousPoint_h

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
	
	template<unsigned short N>
	class IDPoint : public wayne::Point<double,N> {
	public:
		typedef unsigned int id_type;
		typedef double coordinate_type;	
		typedef wayne::Point<double,N> base_type;
	
	private:
		template<unsigned short M=0, typename dummy=void> struct compareLT {
			static bool call(const IDPoint& one, const IDPoint& two) {
				if( one.template elem<M>() < two.template elem<M>() ) {
					return true;
				} else if( one.template elem<M>() > two.template elem<M>() ) {
					return false;
				} else return compareLT<M+1>::call(one, two);
			}
		};
		
		template<typename dummy> struct compareLT<N-1,dummy> {
			static bool call(const IDPoint& one, const IDPoint& two) {
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
		IDPoint() : base_type(), pointID(0) {}
		explicit IDPoint(const id_type nID) : base_type(), pointID(nID) {} //empty set
		IDPoint(std::initializer_list<coordinate_type> coords, const id_type nID)
			: base_type(coords), pointID(nID) {}
		IDPoint(const IDPoint& rhs) : base_type(rhs), pointID(rhs.pointID) {}
		IDPoint& operator=(const IDPoint& rhs) = default;
		virtual ~IDPoint() noexcept = default;
		
		bool operator<(const id_type& id) const { //can this be used by stl containers?
			return pointID < id;
		}
		
		bool operator<(const coordinate_type& x) const { //can this be used by stl containers?
			return base_type::operator[](0) < x;
		}
		
		bool operator<(const IDPoint& rhs) const {
			return compareLT<>::call(*this, rhs);
		}
		
		id_type ID() const { return pointID; }
	}; 
	
	
	template<unsigned short N> class ContinuousSpace; //need inheritance for forward declaration?
	
	template<unsigned short N>
	class ContinuousPoint : public IDPoint<N> {
	public:
		typedef ContinuousSpace<N> space_type;
		typedef IDPoint<N> base_type;
		typedef typename base_type::id_type id_type;
		typedef typename base_type::coordinate_type coordinate_type;
	private:
		static std::atomic<id_type> IDCOUNT;  
		inline static id_type get_new_ID() 
			{ return IDCOUNT.fetch_add(1, std::memory_order_relaxed); }
	
		space_type* space;
		std::mutex point_mutex;
		
		void add_to_space() {
			//space.write_lock(); should client code be responsible for this?
			if( !(space->add_point(this)) ) space = nullptr;
			//space.write_unlock();
		}
		
		void update_space(space_type* new_space) {
			//point_mutex.lock();
			space = new_space;
			//point_mutex.unlock();
		}
		
		friend class ContinuousSpace<N>;
		
	public:
		static space_type SPACE;
		
		explicit ContinuousPoint(const id_type nID=get_new_ID())
			: space(&ContinuousPoint::SPACE), base_type(nID) {
			add_to_space();
		} 
		explicit ContinuousPoint(std::initializer_list<coordinate_type> coords, 
					 const id_type nID=get_new_ID())
			: ContinuousPoint(ContinuousPoint::SPACE, coords, nID) {}
		explicit ContinuousPoint(space_type& cSpace, 
					 const id_type nID=get_new_ID())
			: space(&cSpace), base_type(nID) {
			add_to_space();
		} 
		ContinuousPoint(space_type& cSpace, 
				std::initializer_list<coordinate_type> coords, 
				const id_type nID=get_new_ID()) 
			: base_type(coords, nID), space(&cSpace) {
			add_to_space();
		}
		ContinuousPoint(const ContinuousPoint& rhs)
			: ContinuousPoint(rhs, get_new_ID()) {}
		ContinuousPoint(const ContinuousPoint& rhs, const id_type nID)
			: base_type(rhs), space(rhs.space) {
			base_type::pointID = nID;
			add_to_space();
		}
		ContinuousPoint(const base_type& rhs) 
			: base_type(rhs), space(&SPACE) {
			add_to_space();
		}
		ContinuousPoint& operator=(const ContinuousPoint& rhs) {
			if(this != &rhs) {
				id_type old_id = base_type::ID();
				base_type::operator=(rhs);
				if(space==nullptr) base_type::pointID = get_new_ID();
				else base_type::pointID = old_id; //keep old_id if it was valid before
				space->move_to(*rhs.space, this->ID());
			}
			return *this;
		}
		ContinuousPoint& operator=(const base_type& rhs) {
			if(this != &rhs) { 
				base_type::operator=(rhs);
			}
			return *this;
		}
		virtual ~ContinuousPoint() noexcept {
			if(space != nullptr) space->remove_point(base_type::pointID);
		}
		
		space_type& get_space() { return *space; } 
		
		void lock() { point_mutex.lock(); }
		bool try_lock() { return point_mutex.try_lock(); }
		void unlock() { point_mutex.unlock(); }
		
	}; //class ContinuousPoint
	
	//initialize static members
	template<unsigned short N>
	std::atomic<typename ContinuousPoint<N>::id_type> ContinuousPoint<N>::IDCOUNT(100000);
	
	template<unsigned short N> ContinuousSpace<N> ContinuousPoint<N>::SPACE;
	
} //namespace ben

#endif

