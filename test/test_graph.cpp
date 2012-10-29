/*
	Benoit: a flexible framework for distributed graphs and spaces
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

//to compile and run:
//	g++ -std=c++11 -g -I../src -I../Wayne/src -lgtest -lpthread test_graph.cpp -o test_graph
//	./test_graph

#include <iostream>
#include "gtest/gtest.h"
#include "PullLink.h"
#include "PushLink.h"
#include "Port.h"
#include "Graph.h"
#include "Node.h"

namespace {

	TEST(Links, All) {
		using namespace ben;
		//the following mostly just need to compile, and are separate from the other tests
		PullLink<int, double, 1> link1x;
		PullLink<int, double, 2> link2x;
		PullLink<int, double, 3> link3x;
		PushLink<int, double, 1> link4x;
		PushLink<int, double, 2> link5x;
		
		link1x.set_value(6);
		EXPECT_EQ(6, link1x.get_value());
		link2x.set_value(7);
		EXPECT_EQ(7, link2x.get_value());
		link3x.set_value(8);
		EXPECT_EQ(8, link3x.get_value());
		link4x.set_value(9);
		EXPECT_EQ(9, link4x.get_value());
		link5x.set_value(0);
		EXPECT_EQ(0, link5x.get_value());
		
		//these will actually be used
		PullLink<int, double, 1> link1(1);
		EXPECT_EQ(1, link1.get_value());
		PullLink<int, double, 2> link2(2);
		EXPECT_EQ(2, link2.get_value());
		PullLink<int, double, 3> link3(3);
		EXPECT_EQ(3, link3.get_value());
		
		PushLink<int, double, 1> link4(4);
		EXPECT_EQ(4, link4.get_value());
		PushLink<int, double, 2> link5(5);
		EXPECT_EQ(5, link5.get_value());
		
		double signal1 = 1.23;
		double signal2 = 4.56;
		double signal3 = 7.89;
		double signal4 = 9.51;
		
		//PullLink<1>
		EXPECT_FALSE(link1.is_ready());
		EXPECT_TRUE(link1.push(signal1));
		EXPECT_TRUE(link1.is_ready());
		
		EXPECT_FALSE(link1.push(signal2));
		EXPECT_EQ(signal1, link1.pull());
		EXPECT_FALSE(link1.is_ready());
		
		//PullLink<2>
		EXPECT_FALSE(link2.is_ready());
		EXPECT_TRUE(link2.push(signal1));
		EXPECT_TRUE(link2.is_ready());
		
		EXPECT_TRUE(link2.push(signal2));
		EXPECT_TRUE(link2.is_ready());
		
		EXPECT_FALSE(link2.push(signal3));
		EXPECT_EQ(signal1, link2.pull());
		EXPECT_FALSE(link2.is_ready());
		
		EXPECT_TRUE(link2.push(signal3));
		EXPECT_TRUE(link2.is_ready());
		
		EXPECT_EQ(signal2, link2.pull());
		EXPECT_FALSE(link2.is_ready());
		
		EXPECT_TRUE(link2.push(signal4));
		link2.flush();
		EXPECT_FALSE(link2.is_ready());
			
		//PullLink<n>
		
		//PushLink<1>
		
		//PushLink<n>
		
	}

	TEST(Ports, All) {
		using namespace ben;
		//use PullLink<1>
	}
} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

