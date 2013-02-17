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
#include "Buffer.h"
#include "Port.h"
#include "Graph.h"
#include "DirectedNode.h"

namespace {

	class Buffers : public ::testing::Test {
	protected:
		std::vector<double> signals;
		Buffers() {}
		void PrepareSignals(const unsigned int n) {
			std::default_random_engine gen;
  			std::uniform_real_distribution<double> random_real(0.0,1.0);
			signals.resize(n);
			for(double& x : signals) { x = random_real(gen); }
		}
		template<unsigned int N>
		void TestBuffer(ben::Buffer<double, N>& link) {
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

	TEST_F(Buffers, All) {
		using namespace ben;

		//this mostly just has to compile
		//Buffer<std::vector<double>,3> vector_link;
		
		//these will actually be used	
		Buffer<double, 1> link1;
		Buffer<double, 2> link2;
		Buffer<double, 4> link4;
		
		//Buffer
		TestBuffer<1>(link1); 
		TestBuffer<2>(link2);
		TestBuffer<4>(link4);
	}

	class PortPath : public ::testing::Test {
	protected:
		template<typename I, typename O> 
		void test_construction() { //need a way to customize arguments?
			using namespace ben;
			typedef I input_type;
			typedef O output_type;
	
			//normal construction
			auto input_port_ptr = new input_type(5);
			EXPECT_EQ(5, input_port_ptr->get_address());
			EXPECT_FALSE(input_port_ptr->is_ready());
			EXPECT_TRUE(input_port_ptr->is_ghost());
			
			auto output_port_ptr = new output_type(11);
			EXPECT_EQ(11, output_port_ptr->get_address());
			EXPECT_FALSE(output_port_ptr->is_ready());
			EXPECT_TRUE(output_port_ptr->is_ghost());
		
			//complement constructors
			input_type input_port2(*output_port_ptr, 17);
			EXPECT_EQ(17, input_port2.get_address());
			EXPECT_FALSE(input_port2.is_ready());
			EXPECT_FALSE(output_port_ptr->is_ghost());
			EXPECT_FALSE(input_port2.is_ghost());
			
			output_type output_port2(*input_port_ptr, 19);
			EXPECT_EQ(19, output_port2.get_address());
			EXPECT_FALSE(output_port2.is_ready());
			EXPECT_FALSE(input_port_ptr->is_ghost());
			EXPECT_FALSE(output_port2.is_ghost());
		
			//copy constructor
			input_type input_port3(*input_port_ptr);
			EXPECT_EQ(5, input_port3.get_address());
			EXPECT_FALSE(input_port3.is_ready());
			EXPECT_FALSE(input_port3.is_ghost());
			EXPECT_FALSE(input_port_ptr->is_ghost());

			output_type output_port3(*output_port_ptr);
			EXPECT_EQ(11, output_port3.get_address());
			EXPECT_FALSE(output_port3.is_ready());
			EXPECT_FALSE(output_port3.is_ghost());
			EXPECT_FALSE(output_port_ptr->is_ghost());

			//destruction
			delete input_port_ptr, output_port_ptr;
			input_port_ptr = nullptr;
			output_port_ptr = nullptr;
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_FALSE(input_port2.is_ghost());
			EXPECT_FALSE(output_port3.is_ghost());
			EXPECT_FALSE(input_port3.is_ghost());

			//move constructor
			input_type input_port4(std::move(input_port2));
			EXPECT_EQ(17, input_port4.get_address());
			EXPECT_FALSE(input_port4.is_ghost());
			EXPECT_TRUE(input_port2.is_ghost());

			output_type output_port4(std::move(output_port2));
			EXPECT_EQ(19, output_port4.get_address());
			EXPECT_FALSE(output_port4.is_ghost());
			EXPECT_TRUE(output_port2.is_ghost());

			//move assignment
			input_port2 = std::move(input_port4);
			EXPECT_EQ(17, input_port2.get_address());
			EXPECT_FALSE(input_port2.is_ghost());
			EXPECT_TRUE(input_port4.is_ghost());

			output_port2 = std::move(output_port4);
			EXPECT_EQ(19, output_port2.get_address());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_TRUE(output_port4.is_ghost());
		
			//assignment
			input_port4 = input_port2;
			EXPECT_EQ(17, input_port4.get_address());
			EXPECT_FALSE(input_port2.is_ghost());
			EXPECT_FALSE(input_port4.is_ghost());

			output_port4 = output_port2;
			EXPECT_EQ(19, output_port4.get_address());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_FALSE(output_port4.is_ghost());
		}	
	};

	TEST_F(PortPath, Port_Construction) {
		typedef ben::InPort<ben::Buffer<double, 2>>  input_type;
		typedef ben::OutPort<ben::Buffer<double, 2>> output_type;
		test_construction<input_type, output_type>();
	}
	TEST_F(PortPath, Path_Construction) {
		typedef ben::Path<double> input_type;
		typedef ben::Path<double> output_type;
		//test_construction<input_type, output_type>();
	}

	TEST(Ports, Data) {
		//sending and receiving data
		using namespace ben;
		InPort<Buffer<double,2>> input_port(3);
		OutPort<Buffer<double,2>> output_port(input_port, 5);
		
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
	
	TEST(Paths, Values) {

	}

	class DirectedNodes : public ::testing::Test {
	protected:
		template<typename N>
		void test_construction() {
			using namespace ben;
			typedef N node_type;
			Graph<node_type> graph1;
			
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
		}
		template<typename N>
		void test_add_remove() {
			using namespace ben;
			typedef N node_type;
			auto graph1_ptr = new Graph<node_type>();

			node_type node1(*graph1_ptr, 3), node2(*graph1_ptr, 5), node3(*graph1_ptr, 7), node4(*graph1_ptr, 11);
			EXPECT_TRUE(node1.add_input(3));
			EXPECT_TRUE(node1.contains_input(3));
			EXPECT_TRUE(node1.contains_output(3));
			EXPECT_TRUE(node1.add_input(5)); 
			EXPECT_TRUE(node1.contains_input(5));
			EXPECT_TRUE(node2.contains_output(3));
			EXPECT_FALSE(node1.add_input(5));
			EXPECT_TRUE(node1.add_output(7)); 
			EXPECT_TRUE(node1.contains_output(7));
			EXPECT_TRUE(node3.contains_input(3));
			EXPECT_FALSE(node1.contains_input(11)); 
			EXPECT_FALSE(node1.contains_output(5)); 
			
			EXPECT_TRUE(node4.clone_links(node1));
			EXPECT_TRUE(node4.contains_input(3));
			EXPECT_TRUE(node1.contains_output(11));
			EXPECT_TRUE(node4.contains_output(3));
			EXPECT_TRUE(node1.contains_input(11));
			EXPECT_TRUE(node4.contains_input(5));
			EXPECT_TRUE(node2.contains_output(11));
			EXPECT_TRUE(node4.contains_output(7));
			EXPECT_FALSE(node4.contains_input(11));
			EXPECT_FALSE(node4.contains_output(5));
			
			node_type node5(13);
			EXPECT_FALSE(node5.clone_links(node1));
			EXPECT_FALSE(node5.contains_input(3));

			node4.remove_input(7);
			EXPECT_FALSE(node4.contains_input(7));
			EXPECT_FALSE(node3.contains_output(11));
			node4.remove_output(7);
			EXPECT_FALSE(node4.contains_output(7));	
			EXPECT_FALSE(node3.contains_input(11));	

			node1.clear();
			EXPECT_FALSE(node1.contains_input(3));
			EXPECT_FALSE(node1.contains_input(5));
			EXPECT_FALSE(node1.contains_input(7));
			EXPECT_FALSE(node1.contains_output(7));

			//this test was not included in Graphs.Destruction because it involves links
			delete graph1_ptr;
			graph1_ptr = nullptr;
			EXPECT_EQ(0, node1.size_inputs() + node1.size_outputs());
			EXPECT_EQ(0, node2.size_inputs() + node2.size_outputs());
			EXPECT_EQ(0, node3.size_inputs() + node3.size_outputs());
			EXPECT_EQ(0, node4.size_inputs() + node4.size_outputs());
			EXPECT_FALSE(node1.is_managed());
			EXPECT_FALSE(node2.is_managed());
			EXPECT_FALSE(node3.is_managed());
			EXPECT_FALSE(node4.is_managed());
		}
		template<typename N>
		void test_iteration() {
			using namespace ben;
			typedef N node_type;
			auto graph1_ptr = new Graph<node_type>();

			node_type node1(*graph1_ptr, 3), node2(*graph1_ptr, 5), node3(*graph1_ptr, 7), node4(*graph1_ptr, 11);
			node1.add_input(3);
			node1.add_input(5);
			node1.add_input(7);
			node1.add_output(7);
			node4.clone_links(node1);
			
			for(auto iter = node1.ibegin(); iter != node1.iend(); ++iter) {
				EXPECT_TRUE(graph1_ptr->elem(iter->get_address()).contains_output(3));
				EXPECT_EQ(iter->get_address(), node1.walk(iter).ID());
			}

			for(auto iter = node1.obegin(); iter != node1.oend(); ++iter) {
				EXPECT_TRUE(graph1_ptr->elem(iter->get_address()).contains_input(3));
				EXPECT_EQ(iter->get_address(), node1.walk(iter).ID());
			}
		
			node4.remove_input(node4.find_input(5));
			EXPECT_FALSE(node4.contains_input(5));
			EXPECT_FALSE(node2.contains_output(11));
			EXPECT_TRUE(node4.iend() == node4.find_input(5));

			node4.remove_output(node4.find_output(7));
			EXPECT_FALSE(node4.contains_output(7));
			EXPECT_FALSE(node3.contains_input(11));
			EXPECT_TRUE(node4.oend() == node4.find_output(7));
		
			delete graph1_ptr;
			graph1_ptr = nullptr;
		}
		template<typename N>
		void test_move_destruction() {
			using namespace ben;
			typedef N node_type;
			auto graph1_ptr = new Graph<node_type>();

			node_type node1(*graph1_ptr, 3), node2(*graph1_ptr, 5), node3(*graph1_ptr, 7);
			auto node4_ptr = new node_type(*graph1_ptr, 11);
			node1.add_input(3);
			node1.add_input(5);
			node1.add_input(7);
			node1.add_output(7);
			node4_ptr->clone_links(node1);

			node_type node5 = std::move(node1);
			EXPECT_EQ(3, node5.ID());
			EXPECT_TRUE(graph1_ptr->check(3, &node5));
			EXPECT_FALSE(node1.is_managed());
			EXPECT_EQ(0, node1.size_inputs() + node1.size_outputs());
			EXPECT_TRUE(node5.contains_input(3));
			EXPECT_TRUE(node5.contains_input(5));
			EXPECT_TRUE(node5.contains_input(7));
			EXPECT_TRUE(node5.contains_output(3));
			EXPECT_TRUE(node5.contains_output(7));
			EXPECT_TRUE(node5.contains_input(11));
			EXPECT_TRUE(node5.contains_output(11));

			node1 = std::move(node5);
			EXPECT_TRUE(graph1_ptr->check(3, &node1));
			EXPECT_FALSE(node5.is_managed());
			EXPECT_EQ(0, node5.size_inputs() + node5.size_outputs());
			EXPECT_TRUE(node1.contains_input(3));
			EXPECT_TRUE(node1.contains_input(5));
			EXPECT_TRUE(node1.contains_input(7));
			EXPECT_TRUE(node1.contains_output(3));
			EXPECT_TRUE(node1.contains_output(7));
			EXPECT_TRUE(node1.contains_input(11));
			EXPECT_TRUE(node1.contains_output(11));
			
			delete node4_ptr;
			node4_ptr = nullptr;
			EXPECT_FALSE(graph1_ptr->contains(11));
			EXPECT_FALSE(node1.contains_input(11));
			EXPECT_FALSE(node1.contains_output(11));

			delete graph1_ptr;
			graph1_ptr = nullptr;
		}
	};

	TEST_F(DirectedNodes, MessageNode_Construction) {
		typedef ben::stdMessageNode<double, 1> node_type;
		test_construction<node_type>();
	}
	TEST_F(DirectedNodes, MessageNode_Add_Remove) {
		typedef ben::stdMessageNode<double, 1> node_type;
		test_add_remove<node_type>();
	}
	TEST_F(DirectedNodes, MessageNode_Iteration) {
		typedef ben::stdMessageNode<double, 1> node_type;
		test_iteration<node_type>();
	}
	TEST_F(DirectedNodes, MessageNode_Move_Destruction) {
		typedef ben::stdMessageNode<double, 1> node_type;
		test_move_destruction<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Construction) {
		typedef ben::stdDirectedNode<double> node_type;
		//test_construction<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Add_Remove) {
		typedef ben::stdDirectedNode<double> node_type;
		//test_add_remove<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Iteration) {
		typedef ben::stdDirectedNode<double> node_type;
		//test_iteration<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Move_Destruction) {
		typedef ben::stdDirectedNode<double> node_type;
		//test_move_destruction<node_type>();
	}

	TEST(Graphs, Add_Remove) {
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		Graph<node_type> graph1, graph2;
		
		auto node1_ptr = new node_type(graph1, 3);
		auto node2_ptr = new node_type(5);
		auto node3_ptr = new node_type(7);

		EXPECT_TRUE(graph1.check(3, node1_ptr));
		EXPECT_FALSE(graph1.check(5, node2_ptr));
		EXPECT_TRUE(graph1.add(*node2_ptr));
		EXPECT_TRUE(graph1.check(5, node2_ptr));
		EXPECT_TRUE(graph1.add(*node3_ptr));
		EXPECT_TRUE(graph2.empty());
		EXPECT_TRUE(graph2.add(*node2_ptr));
		EXPECT_TRUE(graph2.check(5, node2_ptr));
		EXPECT_FALSE(graph2.empty());
		EXPECT_EQ(1, graph2.size());
		EXPECT_FALSE(graph1.empty());
		EXPECT_EQ(2, graph1.size());

		EXPECT_TRUE(graph2.remove(5));
		EXPECT_FALSE(graph1.remove(5));
		EXPECT_TRUE(graph2.empty());
		EXPECT_FALSE(node2_ptr->is_managed());
		EXPECT_TRUE(graph2.add(*node2_ptr));
		EXPECT_TRUE(node2_ptr->is_managed());

		delete node1_ptr, node2_ptr, node3_ptr;
	}
	TEST(Graphs, Merge_Move_Destruction) {		
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		Graph<node_type> graph1, graph2;
		auto graph2_ptr = new Graph<node_type>();
		
		auto node1_ptr = new node_type(graph1, 3);
		auto node2_ptr = new node_type(graph1, 5);
		auto node3_ptr = new node_type(*graph2_ptr, 7);
		auto node4_ptr = new node_type(*graph2_ptr, 11);

		EXPECT_TRUE(graph2_ptr->merge_into(graph1));
		EXPECT_EQ(4, graph1.size());
		EXPECT_TRUE(graph2_ptr->empty());
		EXPECT_EQ(&graph1, &(node1_ptr->get_index()));
		EXPECT_EQ(&graph1, &(node2_ptr->get_index()));

		Graph<node_type> graph3(std::move(graph1));
		EXPECT_EQ(4, graph3.size());
		EXPECT_TRUE(graph1.empty());
		EXPECT_EQ(&graph3, &(node1_ptr->get_index()));
		EXPECT_EQ(&graph3, &(node3_ptr->get_index()));

		*graph2_ptr = std::move(graph3);
		EXPECT_EQ(4, graph2_ptr->size());
		EXPECT_TRUE(graph3.empty());
		EXPECT_EQ(graph2_ptr, &(node1_ptr->get_index()));
		EXPECT_EQ(graph2_ptr, &(node3_ptr->get_index()));

		delete graph2_ptr;
		graph2_ptr = nullptr;
		EXPECT_FALSE(node1_ptr->is_managed());
		EXPECT_FALSE(node2_ptr->is_managed());
		EXPECT_FALSE(node3_ptr->is_managed());

		delete node1_ptr, node2_ptr, node3_ptr, node4_ptr;
	}
	TEST(Graphs, Content) {
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		Graph<node_type> graph1, graph2;
		
		auto node1_ptr = new node_type(graph1, 3);
		auto node2_ptr = new node_type(graph1, 5);
		auto node3_ptr = new node_type(graph1, 7);

		for(node_type& x : graph1) {
			EXPECT_TRUE(graph1.check(x.ID(), &x));
		}
		
		EXPECT_TRUE(graph1.contains(7));
		EXPECT_FALSE(graph2.contains(7));

		EXPECT_TRUE(graph2.end() == graph2.find(3));
		auto iter = graph1.begin();
		while(iter->ID() != 7) ++iter;
		EXPECT_TRUE(iter == graph1.find(7));
		EXPECT_TRUE(graph1.end() == graph1.find(4));

		graph1.clear();
		EXPECT_TRUE(graph1.empty());
		EXPECT_FALSE(node1_ptr->is_managed());
		EXPECT_FALSE(node3_ptr->is_managed());

		delete node1_ptr, node2_ptr, node3_ptr;
	}

} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

