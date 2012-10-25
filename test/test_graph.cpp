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

	TEST(PortLink, Construction) {
		using namespace ben;
		PullLink<int, double, 1> link1;
		PullLink<int, double, 2> link2;
		PullLink<int, double, 3> link3;
	}

} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

