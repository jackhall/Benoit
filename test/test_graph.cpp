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
#include <vector>
#include <random>
#include "gtest/gtest.h"
#include "PullLink.h"
#include "PushLink.h"
#include "Port.h"
#include "Graph.h"
#include "Node.h"

namespace {

	class Links : public ::testing::Test {
	protected:
		std::vector<double> signals;
		
		Links() {}
		
		void PrepareSignals(const unsigned int n) {
			std::default_random_engine gen;
  			std::uniform_real_distribution<double> random_real(0.0,1.0);
			signals.resize(n);
			for(double& x : signals) { x = random_real(gen); }
		}
		
		template<unsigned int N>
		void TestLink(ben::PullLink<int, double, N>& link) {
			link.flush();
			EXPECT_FALSE(link.is_ready());
			PrepareSignals(N);
			for(auto x : signals) {
				EXPECT_TRUE(link.push(x)); 
				EXPECT_TRUE(link.is_ready()); 
			}
			EXPECT_FALSE(link.push(1.23));
			
			for(auto x : signals) {
				EXPECT_TRUE(link.is_ready());
				EXPECT_EQ(x, link.pull());
			}
			EXPECT_FALSE(link.is_ready());
			
			if(N>1) {
				PrepareSignals(N/2);
				for(auto x : signals) {
					EXPECT_TRUE(link.push(x));
					EXPECT_TRUE(link.is_ready());
				}
				EXPECT_EQ(signals[0], link.pull());
				double second_signal;
				if(N/2 > 1) second_signal = signals[1];
				
				PrepareSignals(N/2);
				for(auto x : signals) {
					EXPECT_TRUE(link.push(x));
					EXPECT_TRUE(link.is_ready());
				}
				if(N/2 <= 1) second_signal = signals[0];
				EXPECT_EQ(second_signal, link.pull());
			}
		}
		
		template<unsigned int M>
		void TestLink(ben::PushLink<int, double, M>& link) {
			link.flush();
			PrepareSignals(M+1);
			EXPECT_FALSE(link.is_ready());
			EXPECT_TRUE(link.push(1.23));
			EXPECT_TRUE(link.is_ready());
			EXPECT_EQ(1.23, link.pull()); 
			EXPECT_FALSE(link.is_ready());
			//EXPECT_EQ(1.23, link.pull()); //fails with move semantics (no copy left behind)
			
			for(auto x : signals) {
				EXPECT_TRUE(link.push(x));
				EXPECT_TRUE(link.is_ready());
			}
			EXPECT_TRUE(link.is_ready());
			EXPECT_EQ(signals[1], link.pull());
			EXPECT_FALSE(link.is_ready()); 
			
			if(M>1) {
				EXPECT_EQ(signals[2], link.pull());
				EXPECT_TRUE(link.push(1.23));
				EXPECT_TRUE(link.is_ready());
				EXPECT_EQ(signals[2], link.pull());
			}
		}
	};

	TEST_F(Links, All) {
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
		PullLink<int, double, 4> link3(3);
		EXPECT_EQ(3, link3.get_value());
		
		PushLink<int, double, 1> link4(4);
		EXPECT_EQ(4, link4.get_value());
		PushLink<int, double, 2> link5(5);
		EXPECT_EQ(5, link5.get_value());
		
		//PullLink
		TestLink<1>(link1);
		TestLink<2>(link2);
		TestLink<4>(link3); 
		
		//PushLink
		TestLink<1>(link4);
		TestLink<2>(link5); 	
	}

	TEST(Ports, Construction) {
		using namespace ben;
		typedef InPort<PullLink<int, double, 2>>  input_port_type;
		typedef OutPort<PullLink<int, double, 2>> output_port_type;
		
		//normal construction
		input_port_type input_port1(5);
		EXPECT_EQ(5, input_port1.source());
		EXPECT_EQ(0, input_port1.get_value());
		EXPECT_FALSE(input_port1.is_ready());
		
		input_port_type input_port2(7, 10);
		EXPECT_EQ(7, input_port2.source());
		EXPECT_EQ(10, input_port2.get_value());
		EXPECT_FALSE(input_port2.is_ready());
		
		output_port_type output_port1(11);
		EXPECT_EQ(11, output_port1.target());
		EXPECT_EQ(0, output_port1.get_value());
		EXPECT_FALSE(output_port1.is_ready());
		
		output_port_type output_port2(13, 20);
		EXPECT_EQ(13, output_port2.target());
		EXPECT_EQ(20, output_port2.get_value());
		EXPECT_FALSE(output_port2.is_ready());
		
		//complement constructors
		input_port_type input_port3(output_port2, 17);
		EXPECT_EQ(17, input_port3.source());
		EXPECT_EQ(20, input_port3.get_value());
		EXPECT_FALSE(input_port3.is_ready());
		
		output_port_type output_port3(input_port2, 19);
		EXPECT_EQ(19, output_port3.target());
		EXPECT_EQ(10, output_port3.get_value());
		EXPECT_FALSE(output_port3.is_ready());
		
		//copy constructor
		
		//move constructor
		
		//assignment
		
		//move assignment
		
		//destruction
	}
	
	TEST(Ports, Methods) {
		//get_value
		//set_value
		//sending and receiving data
	}
	
} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

