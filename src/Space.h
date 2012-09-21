#ifndef ContinuousSpace_h
#define ContinuousSpace_h

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

#include <map>
#include <set>
#include "Region.h"
#include "Point.h"
#include "Interval.h"
#include "Commons.h"
#include "ContinuousPoint.h"

namespace ben {

	template<unsigned short N>
	class ContinuousSpace : public wayne::Commons {
	public:
		typedef unsigned int id_type;
		typedef double coordinate_type;
		typedef ContinuousPoint<N> local_type;
		typedef IDPoint<N> point_type;
		typedef wayne::Point<double,N> raw_point_type;
		typedef typename std::set<point_type>::iterator iterator;
		static const unsigned short dimensions = N;
	private:
		typedef ContinuousSpace self_type;
		
		//TMP tricks - these are kind of awkward for weird compilation reasons,
		//but they remove increment and comparison computions from loops
		template<unsigned short M=0, typename dummy=void> struct get_low_point {
			static void call(const wayne::Region<double,N>& region, 
					 raw_point_type& point) {
				point.template elem<M>() = region.template elem<M>().lower;
				get_low_point<M+1>::call(region, point);
			}
		};
		
		template<typename dummy> struct get_low_point<N-1,dummy> {
			static void call(const wayne::Region<double,N>& region, 
					 raw_point_type& point) {
				point.template elem<N-1>() = region.template elem<N-1>().lower;
			}
		};
		//-----
		template<unsigned short M=0, typename dummy=void> struct get_high_point {
			static void call(const wayne::Region<double,N>& region,
					 raw_point_type& point) {
				point.template elem<M>() = region.template elem<M>().upper;
				get_high_point<M+1>::call(region, point);
			}
		};
	
		template<typename dummy> struct get_high_point<N-1,dummy> {
			static void call(const wayne::Region<double,N>& region,
					 raw_point_type& point) {
				point.template elem<N-1>() = region.template elem<N-1>().upper;
			}
		};
		//-----
		template<unsigned short M=0, typename dummy=void> struct get_region_around{
			static void call(const raw_point_type& point, 
					 const double radius, 
					 wayne::Region<double,N>& region) {
				region.template elem<M>().lower = point.template elem<M>() - radius;
				region.template elem<M>().upper = point.template elem<M>() + radius;
				get_region_around<M+1>::call(point, radius, region);
			}
		};
	
		template<typename dummy> struct get_region_around<N-1,dummy> {
			static void call(const wayne::Point<double,N>& point, 
					 const double radius, 
					 wayne::Region<double,N>& region) {
				region.template elem<N-1>().lower = point.template elem<N-1>() - radius;
				region.template elem<N-1>().upper = point.template elem<N-1>() + radius;
			}
		};
		//-----
		template<unsigned short M=0, typename dummy=void> struct unwind_distance {
			static double call(const raw_point_type& one, 
			  		   const raw_point_type& two) {
				auto difference = two.template elem<M>() - one.template elem<M>();
				return difference*difference + unwind_distance<M+1>::call(one,two);		
			}
		};
	
		template<typename dummy> struct unwind_distance<N-1,dummy> {
			static double call(const raw_point_type& one, 
			   		   const raw_point_type& two) {
				auto difference = two.template elem<N-1>() - one.template elem<N-1>();
				return difference*difference;	   		
			}
		};
		//-----
		bool within(const raw_point_type& one, 
			    const raw_point_type& two,
			    const double radius) {
			return unwind_distance<>::call(one,two) < (radius*radius);
		}
		
		
		std::set<point_type> points;
		std::map<id_type, local_type*> index;
		
		void update_all_points();
		
	public:
		ContinuousSpace() = default;
		~ContinuousSpace();
		ContinuousSpace(const self_type& rhs) = delete;
		ContinuousSpace(self_type&& rhs);
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs);
		
		
		void update_data();
		
		iterator find(const id_type address);
		bool check(const id_type address, const local_type* local_ptr) const 
			{ return index.find(address)->second == local_ptr; }
		bool contains(const id_type address) const { return index.count(address) > 0; }
		bool empty() const { return index.empty(); }
		
		bool add_point(local_type* point_ptr);
		bool update_point(const id_type address, local_type* point_ptr);
		bool remove_point(const id_type address);
		
		unsigned int size() { return points.size(); }
		
		bool move_to(self_type& destination, const id_type address); //move single point
		void swap_with(self_type& other); //all Nodes
		bool merge_into(self_type& other); 
		void clear();
		
		std::set<point_type> in_region(const wayne::Region<double,N> region) const;
		std::set<point_type> within(const raw_point_type pt, 
					    const double radius) const;
		point_type closest_to(const raw_point_type pt) const;
						    
		iterator begin() { return points.begin(); }
		iterator end()   { return points.end(); }
		//need closest point for search? might have to do voronoi partitions
	}; //class ContinuousSpace

	
	template<unsigned short N>
	ContinuousSpace<N>::ContinuousSpace(ContinuousSpace&& rhs)
		: points( std::move(rhs.points) ), index( std::move(rhs.index) ) {
		update_all_points();
	}
	
	template<unsigned short N>
	ContinuousSpace<N>& ContinuousSpace<N>::operator=(ContinuousSpace&& rhs) {
		if(this != &rhs && this != &local_type::SPACE) {
			merge_into(local_type::SPACE);
			points = std::move(rhs.points);
			index = std::move(rhs.index);
			update_all_points();
		}
	}
	
	template<unsigned short N>
	ContinuousSpace<N>::~ContinuousSpace() {
		if(this != &local_type::SPACE) merge_into(local_type::SPACE); 
	}
	
	template<unsigned short N>
	void ContinuousSpace<N>::update_all_points() {
		auto it = index.begin();
		auto ite = index.end();
		while(it != ite) {
			it->second->update_space(this);
			++it;
		}
	}

	template<unsigned short N>
	void ContinuousSpace<N>::update_data() {
		points.clear();
	
		auto it = index.begin();
		auto ite = index.end();
		while(it != ite) {
			points.insert( *(it->second) );
			++it;
		}
	}
	
	template<unsigned short N>
	typename ContinuousSpace<N>::iterator ContinuousSpace<N>::find(const id_type address) {
		auto it = points.begin();
		auto ite = points.end();
		while(it != ite) {
			if(it->ID() == address) return it;
			++it;
		} return ite;
	}
	
	template<unsigned short N>
	bool ContinuousSpace<N>::add_point(local_type* point_ptr) {
		id_type id = point_ptr->ID();
		if(index.find(id) == index.end()) { //check if that ID already appears
			points.insert(*point_ptr);
			index.insert( std::make_pair(id, point_ptr) );
			return true;
		} else  return false; //shouldn't be trying to add duplicate element
	}
	
	template<unsigned short N>
	bool ContinuousSpace<N>::update_point(const id_type address, local_type* point_ptr) {
		auto it = index.find(address);
		if(it != index.end()) {
			it->second = point_ptr;
			return true;
		} else 	return false;
	}
	
	template<unsigned short N>
	bool ContinuousSpace<N>::remove_point(const id_type address) {
		auto it = index.find(address);
		if(it != index.end()) {
			auto point_ptr = it->second;
			points.erase( find(address) ); 
			index.erase(it);
			//transfer to static space? this would screw up move_point
			//maybe have a private overload taking an iterator?
			return true;
		} else  return false;
	}
	
	template<unsigned short N>
	bool ContinuousSpace<N>::move_to(ContinuousSpace& destination, const id_type address) {
		auto it = index.find(address);
		if(it != index.end()) {
			if(destination.add_point(it->second)) {
				it->second->update_space(&destination);
				return remove_point(address);
			} else return false;
		} else return false;
	}
	
	template<unsigned short N>
	void ContinuousSpace<N>::swap_with(ContinuousSpace& other) {
		auto temp_index = std::move(index);
		auto temp_points = std::move(points);
		
		index = std::move(other.index);
		points = std::move(other.points);
		
		other.index = std::move(temp_index);
		other.points = std::move(temp_points);
		
		update_all_points();
		other.update_all_points();
	}
	
	template<unsigned short N>
	bool ContinuousSpace<N>::merge_into(ContinuousSpace& other) {
		auto it = points.begin();
		auto ite = points.end();
		typename std::map<id_type, local_type*>::iterator index_iter;
		while(it != ite) {
			index_iter = index.find(it->ID());
			if( other.add_point(index_iter->second) ) {
				index.erase(index_iter);
				points.erase(it++);
			} else ++it;
		}
		return points.size() == 0;
	}
	
	template<unsigned short N>
	void ContinuousSpace<N>::clear() {
		auto it = index.begin();
		auto ite = index.end();
		while(it != ite) {
			it->second->space = nullptr;
			++it;
		}
		index.clear();
		points.clear();
	}
	
	template<unsigned short N>
	std::set<typename ContinuousSpace<N>::point_type> 
		ContinuousSpace<N>::in_region(const wayne::Region<double,N> region) const {
		
		point_type lower_point, upper_point;
		get_low_point<N>::call(region, lower_point);
		get_high_point<N>(region, upper_point);
		
		auto lower_iter = points.lower_bound(lower_point);
		auto upper_iter = points.upper_bound(upper_point);
		
		std::set<ContinuousSpace::point_type> output;
		while(lower_iter != upper_iter) {
			if( region.inside(*lower_iter) ) output.insert(*lower_iter); 
			++lower_iter;
		}
		return output;
	}
	
	template<unsigned short N>
	std::set<typename ContinuousSpace<N>::point_type> 
		ContinuousSpace<N>::within(const raw_point_type pt, 
					   const double radius) const {
		wayne::Region<double,N> region;
		get_region_around<N>(pt, radius, region);
		auto output = in_region(region);
		
		auto it = output.begin();
		auto ite = output.end();
		while(it != ite) {
			if(!within<N>(pt, *it, radius)) output.erase(it++);
			else ++it;
		}
		return output;
	}
	
	template<unsigned short N>
	typename ContinuousSpace<N>::point_type 
		ContinuousSpace<N>::closest_to(const raw_point_type pt) const {
		//should probably do voronoi partitions
		//but for now just search linearly
		auto it = points.begin();
		auto ite = points.end();
		point_type output = *it;
		double distance, best_distance=unwind_distance<N>(pt, output); //no need for sqrt
		while(it != ite) {
			distance = unwind_distance<N>(pt, *it);
			if(distance < best_distance) {
				output = *it;
				best_distance = distance;
			}
			++it;
		}
		return output;
	}
} //namespace ben

#endif

