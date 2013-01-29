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
#include "Link.h"
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
		void TestLink(ben::Link<double, N>& link) {
			link.flush(); 
			EXPECT_FALSE(link.is_ready());
			PrepareSignals(N);
			for(auto x : signals) 
				EXPECT_FALSE(link.push(x)); 
			EXPECT_TRUE(link.is_ready());
			EXPECT_EQ(signals.front(), link.pull());
			EXPECT_FALSE(link.is_ready());
			
			if(N>1) { 
				std::vector<double> old_signals(std::move(signals));
				PrepareSignals(3);
				EXPECT_FALSE(link.push(signals[0]));
				EXPECT_TRUE(link.is_ready());
				EXPECT_TRUE(link.push(signals[1]));
				EXPECT_TRUE(link.is_ready());
				if(N < 4)
					EXPECT_EQ(signals[0], link.pull());
				else 
					EXPECT_EQ(old_signals[2], link.pull());
				EXPECT_FALSE(link.is_ready());
				EXPECT_FALSE(link.push(signals[2]));
				EXPECT_TRUE(link.is_ready());
				if(N < 4)
					EXPECT_EQ(signals[1], link.pull());
				else
					EXPECT_EQ(old_signals[3], link.pull());
			}
			if(N>2) {
				link.flush();
				PrepareSignals(3*N);
				for(int i = 0; i<signals.size(); ++i) {
					if(i >= N) EXPECT_EQ(signals[i-N], link.pull());
					EXPECT_FALSE(link.push(signals[i]));
				}
			}
			link.flush();
			EXPECT_FALSE(link.is_ready());
		}
	};

	TEST_F(Links, All) {
		using namespace ben;

		//this mostly just has to compile
		//Link<std::vector<double>,3> vector_link;
		
		//these will actually be used	
		Link<double, 1> link1;
		Link<double, 2> link2;
		Link<double, 4> link4;
		
		//Link
		TestLink<1>(link1); 
		TestLink<2>(link2);
		TestLink<4>(link4);
	}

	TEST(Ports, Construction) {
		using namespace ben;
		typedef InPort<Link<double, 2>>  input_port_type;
		typedef OutPort<Link<double, 2>> output_port_type;
		
		//normal construction
		auto input_port_ptr = new input_port_type(5);
		EXPECT_EQ(5, input_port_ptr->source());
		EXPECT_FALSE(input_port_ptr->is_ready());
		EXPECT_TRUE(input_port_ptr->is_ghost());
		
		auto output_port_ptr = new output_port_type(11);
		EXPECT_EQ(11, output_port_ptr->target());
		EXPECT_FALSE(output_port_ptr->is_ready());
		EXPECT_TRUE(output_port_ptr->is_ghost());
		
		//complement constructors
		input_port_type input_port2(*output_port_ptr, 17);
		EXPECT_EQ(17, input_port2.source());
		EXPECT_FALSE(input_port2.is_ready());
		EXPECT_FALSE(output_port_ptr->is_ghost());
		EXPECT_FALSE(input_port2.is_ghost());
		
		output_port_type output_port2(*input_port_ptr, 19);
		EXPECT_EQ(19, output_port2.target());
		EXPECT_FALSE(output_port2.is_ready());
		EXPECT_FALSE(input_port_ptr->is_ghost());
		EXPECT_FALSE(output_port2.is_ghost());
		
		//copy constructor
		input_port_type input_port3(*input_port_ptr);
		EXPECT_EQ(5, input_port3.source());
		EXPECT_FALSE(input_port3.is_ready());
		EXPECT_FALSE(input_port3.is_ghost());
		EXPECT_FALSE(input_port_ptr->is_ghost());

		output_port_type output_port3(*output_port_ptr);
		EXPECT_EQ(11, output_port3.target());
		EXPECT_FALSE(output_port3.is_ready());
		EXPECT_FALSE(output_port3.is_ghost());
		EXPECT_FALSE(output_port_ptr->is_ghost());

		//destruction
		delete input_port_ptr, output_port_ptr;
		EXPECT_FALSE(output_port2.is_ghost());
		EXPECT_FALSE(input_port2.is_ghost());
		EXPECT_FALSE(output_port3.is_ghost());
		EXPECT_FALSE(input_port3.is_ghost());

		//move constructor
		input_port_type input_port4(std::move(input_port2));
		EXPECT_EQ(17, input_port4.source());
		EXPECT_FALSE(input_port4.is_ghost());
		EXPECT_TRUE(input_port2.is_ghost());

		output_port_type output_port4(std::move(output_port2));
		EXPECT_EQ(19, output_port4.target());
		EXPECT_FALSE(output_port4.is_ghost());
		EXPECT_TRUE(output_port2.is_ghost());

		//move assignment
		input_port2 = std::move(input_port4);
		EXPECT_EQ(17, input_port2.source());
		EXPECT_FALSE(input_port2.is_ghost());
		EXPECT_TRUE(input_port4.is_ghost());

		output_port2 = std::move(output_port4);
		EXPECT_EQ(19, output_port2.target());
		EXPECT_FALSE(output_port2.is_ghost());
		EXPECT_TRUE(output_port4.is_ghost());

		//assignment
		input_port4 = input_port2;
		EXPECT_EQ(17, input_port4.source());
		EXPECT_FALSE(input_port2.is_ghost());
		EXPECT_FALSE(input_port4.is_ghost());

		output_port4 = output_port2;
		EXPECT_EQ(19, output_port4.target());
		EXPECT_FALSE(output_port3.is_ghost());
		EXPECT_FALSE(output_port_ptr->is_ghost());
	}
	
	TEST(Ports, Data) {
		//sending and receiving data
		using namespace ben;
		InPort<Link<double,2>> input_port(3);
		OutPort<Link<double,2>> output_port(input_port, 5);
		
		double signal1(1.23), signal2(4.56), signal3(7.89);

		EXPECT_FALSE(input_port.is_ready());
		EXPECT_FALSE(output_port.push(signal1));
		EXPECT_FALSE(input_port.is_ready());
		EXPECT_FALSE(output_port.push(signal2));
		EXPECT_TRUE(input_port.is_ready());
		EXPECT_TRUE(output_port.push(signal3));
		EXPECT_TRUE(input_port.is_ready());
		EXPECT_EQ(signal2, input_port.pull());
		EXPECT_EQ(0, input_port.pull());
	}

	TEST(Graphs, All) {
		using namespace ben;
		typedef Link<double,1> link_type;
		typedef Node<InPort<link_type>, OutPort<link_type>> node_type;
		Graph<node_type> graph1, graph2;
		
		//basic construction
		auto node1_ptr = new node_type(graph1);
		EXPECT_EQ(0, node1_ptr->size_inputs());
		EXPECT_EQ(0, node1_ptr->size_outputs());
		EXPECT_TRUE(graph1.check(node1_ptr->ID(), node1_ptr));

		auto node2_ptr = new node_type(graph1, 3);
		EXPECT_TRUE(graph1.check(3, node2_ptr));

		auto node3_ptr = new node_type();
		EXPECT_EQ(nullptr, &node3_ptr->get_index());
		auto node4_ptr = new node_type(7);
		EXPECT_EQ(nullptr, &node4_ptr->get_index());

		//graph::add, graph::empty, graph::size
		EXPECT_FALSE(graph1.check(node3_ptr->ID(), node3_ptr));
		EXPECT_TRUE(graph1.add(*node3_ptr));
		EXPECT_TRUE(graph1.check(node3_ptr->ID(), node3_ptr));
		EXPECT_TRUE(graph1.add(*node4_ptr));
		EXPECT_TRUE(graph2.empty());
		EXPECT_TRUE(graph2.add(*node2_ptr));

		EXPECT_FALSE(graph2.empty());
		EXPECT_EQ(1, graph2.size());
		EXPECT_FALSE(graph1.empty());
		EXPECT_EQ(3, graph1.size());

		//graph::remove
		EXPECT_TRUE(graph2.remove(3));
		EXPECT_FALSE(graph1.remove(3));
		EXPECT_TRUE(graph2.empty());
		EXPECT_FALSE(node2_ptr->is_managed());
		EXPECT_TRUE(graph2.add(*node2_ptr));
		EXPECT_TRUE(node2_ptr->is_managed());
		
		//graph::merge_into
		EXPECT_TRUE(graph2.merge_into(graph1));
		EXPECT_EQ(4, graph1.size());
		EXPECT_TRUE(graph2.empty());
		EXPECT_EQ(&graph1, &(node1_ptr->get_index()));
		EXPECT_EQ(&graph1, &(node2_ptr->get_index()));

		//graph move semantics
		Graph<node_type> graph3(std::move(graph1));
		EXPECT_EQ(4, graph3.size());
		EXPECT_TRUE(graph1.empty());
		EXPECT_EQ(&graph3, &(node1_ptr->get_index()));
		EXPECT_EQ(&graph3, &(node2_ptr->get_index()));

		graph1 = std::move(graph3);
		EXPECT_EQ(4, graph1.size());
		EXPECT_TRUE(graph3.empty());

		//graph iteration
		for(node_type& x : graph1) {
			EXPECT_TRUE(graph1.check(x.ID(), &x));
		}
		
		//graph::contains
		EXPECT_TRUE(graph1.contains(7));
		EXPECT_FALSE(graph2.contains(7));

		//graph::find
		EXPECT_TRUE(graph2.end() == graph2.find(3));
		auto iter = graph1.begin();
		while(iter->ID() != 7) ++iter;
		EXPECT_TRUE(iter == graph1.find(7));
		EXPECT_TRUE(graph1.end() == graph1.find(4));

		//graph::clear
		graph1.clear();
		EXPECT_TRUE(graph1.empty());
		EXPECT_FALSE(node1_ptr->is_managed());
		EXPECT_FALSE(node2_ptr->is_managed());
	}

	TEST(Nodes, All) {
		//clone_links method
		//move construction
		//move assignment
		//destruction
		//node::find
		//node::remove
		//node::clear
		//node::contains
		//node iteration (begin/end)
	}
} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

