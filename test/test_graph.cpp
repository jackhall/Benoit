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
		void TestLink(ben::PullLink<double, N>& link) {
			link.flush(); //segfaults
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
		void TestLink(ben::PushLink<double, M>& link) {
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

		//these mostly just have to compile
		typedef PullLink<std::vector<double>,1> vector_link_type;
		EXPECT_EQ(SignalTrait::moveable, vector_link_type::signal_trait);
		
		typedef PullLink<double, 1> double_link_type;
		EXPECT_EQ(SignalTrait::POD, double_link_type::signal_trait);
		
		//need a copyable but not moveable signal_type
		
		//these will actually be used	
		PullLink<double, 1> link1;
		PullLink<double, 2> link2;
		PullLink<double, 4> link3;
		
		PushLink<double, 1> link4;
		PushLink<double, 2> link5;
		
		//PullLink
		std::cout << "segfault soon1\n";
		TestLink<1>(link1); //segfaults
		std::cout << "segfault soon1\n";
		TestLink<2>(link2);
		std::cout << "segfault soon1\n";
		TestLink<4>(link3); //segfaults
		
		//PushLink
		std::cout << "segfault soon1\n";
		TestLink<1>(link4);
		TestLink<2>(link5); 	
	}

	TEST(Ports, Construction) {
		using namespace ben;
		typedef InPort<PullLink<double, 2>>  input_port_type;
		typedef OutPort<PullLink<double, 2>> output_port_type;
		
		//normal construction
		input_port_type input_port1(5);
		EXPECT_EQ(5, input_port1.source());
		EXPECT_FALSE(input_port1.is_ready());
		EXPECT_TRUE(input_port1.is_ghost());
		
		output_port_type output_port1(11);
		EXPECT_EQ(11, output_port1.target());
		EXPECT_FALSE(output_port1.is_ready());
		EXPECT_TRUE(output_port1.is_ghost());
		
		//complement constructors
		input_port_type input_port2(output_port1, 17);
		EXPECT_EQ(17, input_port2.source());
		EXPECT_FALSE(input_port2.is_ready());
		EXPECT_FALSE(output_port1.is_ghost());
		EXPECT_FALSE(input_port2.is_ghost());
		
		output_port_type output_port2(input_port1, 19);
		EXPECT_EQ(19, output_port2.target());
		EXPECT_FALSE(output_port2.is_ready());
		EXPECT_FALSE(input_port1.is_ghost());
		EXPECT_FALSE(output_port2.is_ghost());
		
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

