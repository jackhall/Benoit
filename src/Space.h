#ifndef BenoitSpace_h
#define BenoitSpace_h

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
#include "src/Region.h"
#include "src/Point.h"
#include "src/Interval.h"
#include "src/Commons.h"
#include "Point.h"

namespace ben {

	template<typename T, unsigned short N>
	class Space : public Index< Point<T,N> > {
	private:
		typedef Index< Point<T,N> > base_type;
		typedef Space self_type;

	public:
		typedef T coordinate_type;
		typedef PointAlias<T,N> alias_type;
		typedef wayne::Point<T,N> raw_point_type;
		typedef typename std::set<singleton_type>::iterator iterator;
		static const unsigned short dimensions = N;
	private:
		//TMP tricks - these are kind of awkward for weird compilation reasons,
		//but they remove increment and comparison computions from loops
		template<unsigned short M=0, typename dummy=void> struct get_low_point {
			static void call(const wayne::Region<T,N>& region, 
					 raw_point_type& point) {
				point.template elem<M>() = region.template elem<M>().lower;
				get_low_point<M+1>::call(region, point);
			}
		};
		
		template<typename dummy> struct get_low_point<N-1,dummy> {
			static void call(const wayne::Region<T,N>& region, 
					 raw_point_type& point) {
				point.template elem<N-1>() = region.template elem<N-1>().lower;
			}
		};
		//-----
		template<unsigned short M=0, typename dummy=void> struct get_high_point {
			static void call(const wayne::Region<T,N>& region,
					 raw_point_type& point) {
				point.template elem<M>() = region.template elem<M>().upper;
				get_high_point<M+1>::call(region, point);
			}
		};
	
		template<typename dummy> struct get_high_point<N-1,dummy> {
			static void call(const wayne::Region<T,N>& region,
					 raw_point_type& point) {
				point.template elem<N-1>() = region.template elem<N-1>().upper;
			}
		};
		//-----
		template<unsigned short M=0, typename dummy=void> struct get_region_around{
			static void call(const raw_point_type& point, 
					 const double radius, 
					 wayne::Region<T,N>& region) {
				region.template elem<M>().lower = point.template elem<M>() - radius;
				region.template elem<M>().upper = point.template elem<M>() + radius;
				get_region_around<M+1>::call(point, radius, region);
			}
		};
	
		template<typename dummy> struct get_region_around<N-1,dummy> {
			static void call(const raw_point_type& point, 
					 const double radius, 
					 wayne::Region<T,N>& region) {
				region.template elem<N-1>().lower = point.template elem<N-1>() - radius;
				region.template elem<N-1>().upper = point.template elem<N-1>() + radius;
			}
		};	
		
		std::set<alias_type> points;
		
	public:
		Space() = default;
		Space(const self_type& rhs) = delete;
		Space(self_type&& rhs);
		self_type& operator=(const self_type& rhs) = delete;
		self_type& operator=(self_type&& rhs);
		virtual ~Space() { clear(); }
		
		void update_data();
		
		alias_type get_alias(const id_type address);
		
		virtual bool add(reference& x);
		virtual bool remove(const id_type address);
		virtual void clear() { base_type::clear(); points.clear(); }
		
		virtual bool move_to(self_type& destination, const id_type address); //move single point
		virtual void swap_with(self_type& other); //all Nodes
		virtual bool merge_into(self_type& other); 
		
		std::set<singleton_type> in_region(const wayne::Region<double,N> region) const;
		std::set<singleton_type> within(const raw_point_type pt, 
					    const double radius) const;
		singleton_type closest_to(const raw_point_type pt) const;
						    
		iterator begin() { return points.begin(); }
		iterator end()   { return points.end(); }
		//need closest point for search? might have to do voronoi partitions
	}; //class Space

	
	template<typename T, unsigned short N>
	Space<T,N>::Space(Space&& rhs)
		: base_type(rhs), points( std::move(rhs.points) ) {
		update_all_points();
	}
	
	template<typename T, unsigned short N>
	Space<T,N>& Space<T,N>::operator=(Space&& rhs) {
		if(this != &rhs) {
			clear();
			points = std::move(rhs.points);
			base_type::operator=(rhs);
			update_all_points();
		}
	}

	template<typename T, unsigned short N>
	void Space<T,N>::update_data() {
		points.clear();
		for(auto x& : index) points.insert( *(x.second) );
	}
	
	template<typename T, unsigned short N>
	typename Space<T,N>::iterator Space<T,N>::find(const id_type address) { //not redundant; rename?
		auto it=points.begin()
		for(auto ite=points.end(); it!=ite; ++it)
			if(it->ID() == address) return it;
		return it;
	}
	
	template<typename T, unsigned short N>
	bool Space<T,N>::add(singleton_type& x) {
		if( base_type::add(x) ) { 
			points.insert(*point_ptr);
			return true;
		} else  return false; //shouldn't be trying to add duplicate element
	}
	
	template<typename T, unsigned short N>
	bool Space<T,N>::remove(const id_type address) {
		auto alias = get_alias(address);
		if( base_type::remove(address) ) {
			points.erase(alias); 
			return true;
		} else  return false;
	}
	
	template<typename T, unsigned short N>
	bool Space<T,N>::move_to(Space& destination, const id_type address) {
		auto it = index.find(address);
		if(it != index.end()) {
			if(destination.add_point(it->second)) {
				it->second->update_space(&destination);
				return remove_point(address);
			} else return false;
		} else return false;
	}
	
	template<typename T, unsigned short N>
	void Space<T,N>::swap_with(Space& other) {
		auto temp_index = std::move(index);
		auto temp_points = std::move(points);
		
		index = std::move(other.index);
		points = std::move(other.points);
		
		other.index = std::move(temp_index);
		other.points = std::move(temp_points);
		
		update_all_points();
		other.update_all_points();
	}
	
	template<typename T, unsigned short N>
	bool Space<T,N>::merge_into(Space& other) {
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
	
	template<typename T, unsigned short N>
	std::set<typename Space<T,N>::singleton_type> 
	    Space<T,N>::in_region(const wayne::Region<double,N> region) const {
		
		singleton_type lower_point, upper_point;
		get_low_point<N>::call(region, lower_point);
		get_high_point<N>(region, upper_point);
		
		auto lower_iter = points.lower_bound(lower_point);
		auto upper_iter = points.upper_bound(upper_point);
		
		std::set<Space::singleton_type> output;
		while(lower_iter != upper_iter) {
			if( region.inside(*lower_iter) ) output.insert(*lower_iter); 
			++lower_iter;
		}
		return output;
	}
	
	template<typename T, unsigned short N>
	std::set<typename Space<T,N>::singleton_type> 
	    Space<T,N>::within(const raw_point_type pt, const double radius) const {
		wayne::Region<double,N> region;
		get_region_around<N>(pt, radius, region);
		auto output = in_region(region);
		
		for(auto it=output.begin(), auto it=output.end(); it!=ite; ++it)
			if(!within<N>(pt, *it, radius)) output.erase(it);
		
		return output;
	}
	
	template<typename T, unsigned short N>
	typename Space<T,N>::singleton_type 
	    Space<T,N>::closest_to(const raw_point_type pt) const {
		//for now just search linearly
		//other options:
		//	delaunay tesselation
		//	iterative neighborhood search 
		singleton_type output = *(points.begin());
		double distance, best_distance = unwind_distance<N>(pt, output); //no need for sqrt
		for(auto x& : points) { 
			distance = unwind_distance<N>(pt, x);
			if(distance < best_distance) {
				output = x;
				best_distance = distance;
			}
		}
		return output;
	}
} //namespace ben

#endif

