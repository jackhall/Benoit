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

//to test Eiffel, run the following from the test directory:
//	g++ -std=c++11 -g -I../src -I../Wayne -lgtest -lpthread test_benoitv2.cpp -o test_benoit
//	./test_benoit

#include "gtest/gtest.h"
#include "Space.h"
#include "Point.h"
#include <iostream>

namespace {
	
	TEST(Construction, Spaces) {
		using namespace eiffel;
		Space<double,3>* space1_ptr = new Space<double,3>();
		EXPECT_TRUE(space1_ptr->empty());
		
		Space<double,3> space2; 
		Point<double,3> point1(space2), 
				   point2(space2, {-1.1, 2.2, 3.3}), 
				   point3(space2, {9.3, -8.4, -7.5});
		EXPECT_TRUE(space2.contains( point1.ID() ));
		EXPECT_TRUE(space2.contains( point2.ID() ));
		EXPECT_TRUE(space2.contains( point3.ID() ));
	
		Space<double,3>* space3_ptr = new Space<double,3>( std::move(space2) );
		EXPECT_TRUE(space3_ptr->contains( point1.ID() ));
		EXPECT_TRUE(space3_ptr->contains( point2.ID() ));
		EXPECT_TRUE(space3_ptr->contains( point3.ID() ));
		EXPECT_FALSE(space2.contains( point1.ID() ));
		EXPECT_FALSE(space2.contains( point2.ID() ));
		EXPECT_FALSE(space2.contains( point3.ID() ));
		EXPECT_TRUE(space2.empty());
		
		*space1_ptr = std::move(*space3_ptr);
		EXPECT_TRUE(space1_ptr->contains( point1.ID() ));
		EXPECT_TRUE(space1_ptr->contains( point2.ID() ));
		EXPECT_TRUE(space1_ptr->contains( point3.ID() ));
		EXPECT_FALSE(space3_ptr->contains( point1.ID() ));
		EXPECT_FALSE(space3_ptr->contains( point2.ID() ));
		EXPECT_FALSE(space3_ptr->contains( point3.ID() ));
		EXPECT_TRUE(space3_ptr->empty());
		
		delete space1_ptr, space3_ptr;
	}

	TEST(Construction, Points) {
		using namespace ben;
		
		//PointAlias
		PointAlias<double,3>* raw_pt1_ptr = new PointAlias<double,3>();
		for(int i=2; i>=0; --i) EXPECT_EQ(0.0, (*raw_pt1_ptr)[i]);
		EXPECT_EQ(0, raw_pt1_ptr->ID());
		
		PointAlias<double,3>* raw_pt2_ptr = new PointAlias<double,3>(1); 
		for(int i=2; i>=0; --i) EXPECT_EQ(0.0, (*raw_pt2_ptr)[i]);
		EXPECT_EQ(1, raw_pt2_ptr->ID());

		PointAlias<double,3>* raw_pt3_ptr = new PointAlias<double,3>({-1.234, 5.678, -9.0}, 2); //not initializing
		EXPECT_EQ(-1.234, (*raw_pt3_ptr)[0]);
		EXPECT_EQ(5.678, (*raw_pt3_ptr)[1]);
		EXPECT_EQ(-9.0, (*raw_pt3_ptr)[2]);
		EXPECT_EQ(2, raw_pt3_ptr->ID());

		PointAlias<double,3>* raw_pt4_ptr = new PointAlias<double,3>(*raw_pt3_ptr);
		EXPECT_EQ(-1.234, (*raw_pt4_ptr)[0]);
		EXPECT_EQ(5.678, (*raw_pt4_ptr)[1]);
		EXPECT_EQ(-9.0, (*raw_pt4_ptr)[2]);
		EXPECT_EQ(2, raw_pt4_ptr->ID());
		EXPECT_EQ(*raw_pt3_ptr, *raw_pt4_ptr);

		*raw_pt2_ptr = *raw_pt4_ptr;
		EXPECT_EQ(-1.234, (*raw_pt2_ptr)[0]);
		EXPECT_EQ(5.678, (*raw_pt2_ptr)[1]);
		EXPECT_EQ(-9.0, (*raw_pt2_ptr)[2]);
		EXPECT_EQ(2, raw_pt2_ptr->ID());

		//Point
		//std::cout << "1 ";
		/*
		Point<double,3>* pt1_ptr = new Point<double,3>(); 
		for(int i=2; i>=0; --i) EXPECT_EQ(0.0, (*pt1_ptr)[i]);
		EXPECT_EQ(100000, pt1_ptr->ID());
		EXPECT_TRUE(Point<double,3>::SPACE.check(100000, pt1_ptr));
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt1_ptr->get_space()));
		
		Point<double,3>* pt2_ptr = new Point<double,3>(100000); //initial value of id generator
		for(int i=2; i>=0; --i) EXPECT_EQ(0.0, (*pt2_ptr)[i]);
		EXPECT_EQ(100000, pt2_ptr->ID());
		EXPECT_EQ(nullptr, &(pt2_ptr->get_space()));
		EXPECT_FALSE(Point<double,3>::SPACE.check(100000, pt2_ptr));
		*pt2_ptr = *raw_pt3_ptr;
		EXPECT_EQ(-1.234, (*pt2_ptr)[0]);
		EXPECT_EQ(5.678, (*pt2_ptr)[1]);
		EXPECT_EQ(-9.0, (*pt2_ptr)[2]);
		EXPECT_NE(*Point<double,3>::SPACE.find(100000), *pt2_ptr);
		
		Point<double,3>* pt3_ptr = new Point<double,3>({2.6, 5.1, -0.56});
		EXPECT_EQ(2.6, (*pt3_ptr)[0]);
		EXPECT_EQ(5.1, (*pt3_ptr)[1]);
		EXPECT_EQ(-.56, (*pt3_ptr)[2]);
		EXPECT_EQ(100001, pt3_ptr->ID());
		EXPECT_TRUE(Point<double,3>::SPACE.check(100001, pt3_ptr));
		EXPECT_EQ(*Point<double,3>::SPACE.find(100001), *pt3_ptr);
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt3_ptr->get_space())); 
		
		Point<double,3>* pt4_ptr = new Point<double,3>({3.14, -2.7, 6.22}, 3);
		EXPECT_EQ(3.14, (*pt4_ptr)[0]);
		EXPECT_EQ(-2.7, (*pt4_ptr)[1]);
		EXPECT_EQ(6.22, (*pt4_ptr)[2]);
		EXPECT_EQ(3, pt4_ptr->ID());
		EXPECT_EQ(*Point<double,3>::SPACE.find(3), *pt4_ptr);
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt4_ptr->get_space()));
		EXPECT_TRUE(Point<double,3>::SPACE.check(3, pt4_ptr));
		
		Point<double,3>* pt5_ptr = new Point<double,3>(*pt4_ptr);
		EXPECT_EQ(3.14, (*pt5_ptr)[0]);
		EXPECT_EQ(-2.7, (*pt5_ptr)[1]);
		EXPECT_EQ(6.22, (*pt5_ptr)[2]);
		EXPECT_EQ(100002, pt5_ptr->ID()); 
		EXPECT_TRUE(Point<double,3>::SPACE.check(100002, pt5_ptr));
		EXPECT_EQ(*Point<double,3>::SPACE.find(100002), *pt5_ptr);
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt5_ptr->get_space()));
		
		Point<double,3>* pt6_ptr = new Point<double,3>(*pt4_ptr, 4);
		EXPECT_EQ(3.14, (*pt6_ptr)[0]);
		EXPECT_EQ(-2.7, (*pt6_ptr)[1]);
		EXPECT_EQ(6.22, (*pt6_ptr)[2]);
		EXPECT_EQ(4, pt6_ptr->ID());
		EXPECT_TRUE(Point<double,3>::SPACE.check(4, pt6_ptr));
		EXPECT_EQ(*Point<double,3>::SPACE.find(4), *pt6_ptr);
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt6_ptr->get_space()));
		
		Point<double,3>* pt7_ptr = new Point<double,3>(*raw_pt3_ptr);
		EXPECT_EQ(-1.234, (*pt7_ptr)[0]);
		EXPECT_EQ(5.678, (*pt7_ptr)[1]);
		EXPECT_EQ(-9.0, (*pt7_ptr)[2]);
		EXPECT_EQ(2, pt7_ptr->ID());
		EXPECT_EQ(*raw_pt3_ptr, *pt7_ptr);
		EXPECT_TRUE(Point<double,3>::SPACE.check(2, pt7_ptr));
		EXPECT_EQ(*Point<double,3>::SPACE.find(2), *pt7_ptr);
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt7_ptr->get_space()));
		
		*pt1_ptr = *pt7_ptr;
		EXPECT_EQ(-1.234, (*pt1_ptr)[0]);
		EXPECT_EQ(5.678, (*pt1_ptr)[1]);
		EXPECT_EQ(-9.0, (*pt1_ptr)[2]);
		EXPECT_EQ(100000, pt1_ptr->ID());
		EXPECT_EQ(*pt1_ptr, *pt7_ptr); //should call wayne::Point::operator==
		EXPECT_TRUE(Point<double,3>::SPACE.check(100000, pt1_ptr));
		EXPECT_NE(*Point<double,3>::SPACE.find(100000), *pt7_ptr); //space hasn't been updated
		EXPECT_EQ(&Point<double,3>::SPACE, &(pt1_ptr->get_space()));
		
		//Point<double,3>::SPACE.clear(); //this should not be necessary, but it is
		
		delete raw_pt1_ptr, raw_pt2_ptr, raw_pt3_ptr, raw_pt4_ptr;
		delete pt1_ptr, pt2_ptr, pt3_ptr, pt4_ptr, pt5_ptr, pt6_ptr, pt7_ptr; 
		
		EXPECT_TRUE(Point<double,3>::SPACE.empty());
		//test stack construction too? */
	}

	class PointManagement : public ::testing::Test {
	protected:
		eiffel::Space<double,3> space1, space2;
		eiffel::Point<double,3> point1, point2;
		
		PointManagement() : space1(), space2(), 
				    point1(space1, {1.23, -4.56, 7.89}, 1), 
				    point2(space1, {-9.87, 6.54, -3.21}, 2) {}
	};

	TEST_F(PointManagement, Moving) {
		//test for Spaces: move_to, swap_with, merge_into, empty, contains,
		//		   check, find, size
		using namespace eiffel;
		EXPECT_TRUE(space2.empty());
		EXPECT_EQ(0, space2.size());
		EXPECT_EQ(2, space1.size());
		EXPECT_TRUE(space1.contains(1));
		EXPECT_TRUE(space1.contains(2));
		EXPECT_TRUE(space1.check(1, &point1));
		EXPECT_TRUE(space1.check(2, &point2));
		
		space1.move_to(space2, point1.ID());
		EXPECT_EQ(1, space2.size());
		EXPECT_EQ(1, space1.size());
		EXPECT_TRUE(space2.contains(1));
		EXPECT_TRUE(space1.contains(2));
		EXPECT_TRUE(space2.check(1, &point1));
		EXPECT_TRUE(space1.check(2, &point2));
		
		space2.swap_with(space1);
		EXPECT_EQ(1, space2.size());
		EXPECT_EQ(1, space1.size());
		EXPECT_TRUE(space2.contains(2));
		EXPECT_TRUE(space1.contains(1));
		EXPECT_TRUE(space1.check(1, &point1));
		EXPECT_TRUE(space2.check(2, &point2));
		
		space2.merge_into(space1);
		EXPECT_TRUE(space2.empty());
		EXPECT_EQ(0, space2.size());
		EXPECT_EQ(2, space1.size());
		EXPECT_TRUE(space1.contains(1));
		EXPECT_TRUE(space1.contains(2));
		EXPECT_TRUE(space1.check(1, &point1));
		EXPECT_TRUE(space1.check(2, &point2));
		
		PointAlias<double,3> raw_point({1.23, -4.56, 7.89}, 10);
		auto it = space1.begin();
		auto ite = space1.end();
		EXPECT_TRUE(ite != it);
		
		++it;
		auto iter = space1.find(1);
		EXPECT_TRUE(it == iter);
		EXPECT_EQ(raw_point, *iter);
		++it;
		EXPECT_TRUE(ite == it);
		
		//test bad conditions? (redundant ids, etc.)
	}
	
	TEST_F(PointManagement, Data) {
		//test for Points: operator<, operator==, operator!=
		//test for Spaces: update_data, add_point, update_point, remove_point, 
		//		   in_region, within, closest_to
		using namespace eiffel;
		EXPECT_FALSE(point1 < point2);
		EXPECT_TRUE(point2 < point1);
		
		PointAlias<double,3> raw_point({1.23, -5.56, 7.89}, 1);
		EXPECT_TRUE(raw_point < point1);
		EXPECT_FALSE(raw_point < point2);
		
		raw_point[1] = -4.56;
		EXPECT_FALSE(raw_point < point1);
		EXPECT_TRUE(raw_point == point1);
		EXPECT_TRUE(raw_point != point2);
		EXPECT_FALSE(raw_point != point1);
		EXPECT_FALSE(raw_point == point2);
		
		point1[0] = 0.12;
		EXPECT_EQ(raw_point, *space1.find(1));
		space1.update_data();
		EXPECT_NE(raw_point, *space1.find(1));
		
		Point<double,3> point3({0.12, 3.45, 6.78}, 1);
		//EXPECT_FALSE( Point<double,3>::SPACE.move_to(space1, 1) ); //move_to calls add_point
		//EXPECT_TRUE( Point<double,3>::SPACE.check(1, &point3) );
		//EXPECT_EQ(&Point<double,3>::SPACE, &point3.get_space());
		EXPECT_NE(&space1, &point3.get_space());
		EXPECT_FALSE( space1.check(1, &point3) );
		
		Point<double,3> point4(point3, 4);
		//EXPECT_TRUE( Point<double,3>::SPACE.move_to(space1, 4) );
		//EXPECT_FALSE( Point<double,3>::SPACE.check(4, &point4) );
		//EXPECT_NE(&Point<double,3>::SPACE, &point4.get_space());
		EXPECT_EQ(&space1, &point4.get_space());
		EXPECT_TRUE( space1.check(4, &point4) );
		
		//update_point, remove_point, in_region, within, closest_to
		
		
	}

} //anonymous namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

