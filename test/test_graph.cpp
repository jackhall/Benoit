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
#include "UndirectedNode.h"

namespace {

	class Buffers : public ::testing::Test {
	protected:
		std::vector<double> signals;
		void PrepareSignals(const unsigned int n) {
			std::default_random_engine gen;
  			std::uniform_real_distribution<double> random_real(0.0,1.0);
			signals.resize(n);
			for(double& x : signals) { x = random_real(gen); }
		}
		template<unsigned int N>
		void TestBuffer(ben::Buffer<double, N>& link) {
			double test_signal;
			EXPECT_FALSE(link.pull(test_signal));
			PrepareSignals(3*N);
			for(int i = 0; i<signals.size(); ++i) {
				if(i >= N) {
					EXPECT_TRUE(link.pull(test_signal));
					EXPECT_EQ(signals[i-N], test_signal);
				}
				EXPECT_FALSE(link.pull(test_signal));
				EXPECT_TRUE(link.push(signals[i]));
			}
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
		{
			SCOPED_TRACE("Buffer length = 1");
			TestBuffer<1>(link1); 
		} {
			SCOPED_TRACE("Buffer length = 2");
			TestBuffer<2>(link2);
		} {
			SCOPED_TRACE("Buffer length = 4");
			TestBuffer<4>(link4);
		}
	}


	class PortPath : public ::testing::Test {
	protected:
		template<typename I, typename O, typename... Args> 
		void test_construction(Args... args) { 
			using namespace ben;
			typedef I input_type;
			typedef O output_type;
	
			//normal construction
			auto input_port_ptr = new input_type(5, args...);
			EXPECT_EQ(5, input_port_ptr->get_address());
			EXPECT_TRUE(input_port_ptr->is_ghost());
			
			auto output_port_ptr = new output_type(11, args...);
			EXPECT_EQ(11, output_port_ptr->get_address());
			EXPECT_TRUE(output_port_ptr->is_ghost());
		
			//complement constructors
			input_type input_port2(*output_port_ptr, 17);
			EXPECT_EQ(17, input_port2.get_address());
			EXPECT_FALSE(output_port_ptr->is_ghost());
			EXPECT_FALSE(input_port2.is_ghost());
			
			output_type output_port2(*input_port_ptr, 19);
			EXPECT_EQ(19, output_port2.get_address());
			EXPECT_FALSE(input_port_ptr->is_ghost());
			EXPECT_FALSE(output_port2.is_ghost());
		
			//copy constructor
			input_type input_port3(*input_port_ptr);
			EXPECT_EQ(5, input_port3.get_address());
			EXPECT_FALSE(input_port3.is_ghost());
			EXPECT_FALSE(input_port_ptr->is_ghost());

			output_type output_port3(*output_port_ptr);
			EXPECT_EQ(11, output_port3.get_address());
			EXPECT_FALSE(output_port3.is_ghost());
			EXPECT_FALSE(output_port_ptr->is_ghost());

			//destruction
			delete input_port_ptr; input_port_ptr = nullptr;
			delete output_port_ptr; output_port_ptr = nullptr;
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_FALSE(input_port2.is_ghost());
			EXPECT_FALSE(output_port3.is_ghost());
			EXPECT_FALSE(input_port3.is_ghost());

			//move constructor
			input_type input_port4(std::move(input_port2));
			EXPECT_EQ(17, input_port4.get_address());
			EXPECT_FALSE(input_port4.is_ghost());
			EXPECT_TRUE(input_port2.is_ghost()); //fails

			output_type output_port4(std::move(output_port2));
			EXPECT_EQ(19, output_port4.get_address());
			EXPECT_FALSE(output_port4.is_ghost());
			EXPECT_TRUE(output_port2.is_ghost()); //fails

			//move assignment
			input_port2 = std::move(input_port4);
			EXPECT_EQ(17, input_port2.get_address());
			EXPECT_FALSE(input_port2.is_ghost());
			EXPECT_TRUE(input_port4.is_ghost()); //fails

			output_port2 = std::move(output_port4);
			EXPECT_EQ(19, output_port2.get_address());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_TRUE(output_port4.is_ghost()); //fails
		
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
		SCOPED_TRACE("Ports");
		typedef ben::InPort<ben::Buffer<double, 2>>  input_type;
		typedef ben::OutPort<ben::Buffer<double, 2>> output_type;
		test_construction<input_type, output_type>();
	}
	TEST_F(PortPath, Path_Construction) {
		SCOPED_TRACE("Paths");
		typedef ben::Path<double> input_type;
		typedef ben::Path<double> output_type;
		test_construction<input_type, output_type>(3.14159);
	}

	TEST(Ports, Data) {
		//sending and receiving data
		using namespace ben;
		InPort<Buffer<double,2>> input_port(3);
		OutPort<Buffer<double,2>> output_port(input_port, 5);
		
		double test_signal, signal1(1.23), signal2(4.56), signal3(7.89);

		EXPECT_FALSE(input_port.pull(test_signal));
		EXPECT_TRUE(output_port.push(signal1));
		EXPECT_FALSE(input_port.pull(test_signal));
		EXPECT_TRUE(output_port.push(signal2));
		EXPECT_FALSE(output_port.push(signal3));
		EXPECT_TRUE(input_port.pull(test_signal));
		EXPECT_EQ(signal2, test_signal);
	}
	
	TEST(Paths, Values) {
		//getting and setting values, verifying clones
		using namespace ben;
		Path<double> path1(3, 0.0);
		Path<double> path2(path1, 5);

		double value1(1.23), value2(4.56);
		
		EXPECT_EQ(0, path1.get_value());
		EXPECT_EQ(0, path2.get_value());
		path1.set_value(value1);
		EXPECT_EQ(value1, path1.get_value());
		EXPECT_EQ(value1, path2.get_value());
		path2.set_value(value2);
		EXPECT_EQ(value2, path1.get_value());
		EXPECT_EQ(value2, path2.get_value());

		auto path3 = path1.clone(7);
		Path<double> path4(path3, 11);
		EXPECT_EQ(value2, path3.get_value());
		EXPECT_EQ(value2, path4.get_value());
		path4.set_value(value1);
	}


	class DirectedNodes : public ::testing::Test {
	protected:
		template<typename N>
		void test_construction() {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();
			
			auto node1_ptr = new node_type(graph1_ptr);
			EXPECT_EQ(0, node1_ptr->inputs.size());
			EXPECT_EQ(0, node1_ptr->outputs.size());
			EXPECT_TRUE(graph1_ptr->check(node1_ptr->ID(), node1_ptr));

			auto node2_ptr = new node_type(graph1_ptr, 3);
			EXPECT_TRUE(graph1_ptr->check(3, node2_ptr));

			auto node3_ptr = new node_type();
			EXPECT_FALSE(node3_ptr->is_managed());
			auto node4_ptr = new node_type(7);
			EXPECT_FALSE(node4_ptr->is_managed());
		}
		template<typename N, typename... Args>
		void test_add_remove(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7), node4(graph1_ptr, 11);
			EXPECT_TRUE(node1.add_input(3, args...)); //creates Port/Path
			EXPECT_TRUE(node1.inputs.contains(3));
			EXPECT_TRUE(node1.outputs.contains(3));
			EXPECT_TRUE(node1.add_input(5, args...)); //creates Port/Path
			EXPECT_TRUE(node1.inputs.contains(5));
			EXPECT_TRUE(node2.outputs.contains(3));
			EXPECT_FALSE(node1.add_input(5, args...)); //creates Port/Path
			EXPECT_TRUE(node1.add_output(7, args...)); //creates Port/Path
			EXPECT_TRUE(node1.outputs.contains(7));
			EXPECT_TRUE(node3.inputs.contains(3));
			EXPECT_FALSE(node1.inputs.contains(11)); 
			EXPECT_FALSE(node1.outputs.contains(5)); 
			
			EXPECT_TRUE(node4.mirror(node1)); //clone_links cannot be well-defined for DirectedNode
			EXPECT_FALSE(node4.inputs.contains(3));
			EXPECT_FALSE(node1.outputs.contains(11));
			EXPECT_FALSE(node4.outputs.contains(3));
			EXPECT_FALSE(node1.inputs.contains(11));
			EXPECT_TRUE(node4.inputs.contains(5));
			EXPECT_TRUE(node2.outputs.contains(11));
			EXPECT_TRUE(node4.outputs.contains(7));
			EXPECT_TRUE(node4.inputs.contains(11));
			EXPECT_TRUE(node4.outputs.contains(11));
			EXPECT_FALSE(node4.outputs.contains(5));
			
			node_type node5(13);
			EXPECT_FALSE(node5.mirror(node1));
			EXPECT_FALSE(node5.inputs.contains(3));

			node4.remove_input(7);
			EXPECT_FALSE(node4.inputs.contains(7));
			EXPECT_FALSE(node3.outputs.contains(11));
			node4.remove_output(7);
			EXPECT_FALSE(node4.outputs.contains(7));	
			EXPECT_FALSE(node3.inputs.contains(11));	

			node1.clear();
			EXPECT_FALSE(node1.inputs.contains(3));
			EXPECT_FALSE(node1.inputs.contains(5));
			EXPECT_FALSE(node1.inputs.contains(7));
			EXPECT_FALSE(node1.outputs.contains(7));

			//testing ownership semantics of graph
			graph1_ptr.reset();
			EXPECT_EQ(0, node1.inputs.size() + node1.outputs.size());
			EXPECT_EQ(1, node2.inputs.size() + node2.outputs.size());
			EXPECT_EQ(0, node3.inputs.size() + node3.outputs.size());
			EXPECT_EQ(3, node4.inputs.size() + node4.outputs.size());
			EXPECT_TRUE(node1.is_managed());
			EXPECT_TRUE(node2.is_managed());
			EXPECT_TRUE(node3.is_managed());
			EXPECT_TRUE(node4.is_managed());
		}
		template<typename N, typename... Args>
		void test_iteration(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7), node4(graph1_ptr, 11);
			node1.add_input(3, args...);
			node1.add_input(5, args...);
			node1.add_input(7, args...);
			node1.add_output(7, args...);
			node4.mirror(node1); //changed from clone_links, may need to adjust tests
			
			for(auto iter = node1.inputs.begin(); iter != node1.inputs.end(); ++iter) {
				EXPECT_TRUE(graph1_ptr->elem(iter->get_address()).outputs.contains(3));
				EXPECT_EQ(iter->get_address(), node1.walk(iter).ID());
			}

			for(auto iter = node1.outputs.begin(); iter != node1.outputs.end(); ++iter) {
				EXPECT_TRUE(graph1_ptr->elem(iter->get_address()).inputs.contains(3));
				EXPECT_EQ(iter->get_address(), node1.walk(iter).ID());
			}
		
			node4.remove_input(node4.inputs.find(5));
			EXPECT_FALSE(node4.inputs.contains(5));
			EXPECT_FALSE(node2.outputs.contains(11));
			EXPECT_TRUE(node4.inputs.end() == node4.inputs.find(5));

			node4.remove_output(node4.outputs.find(7));
			EXPECT_FALSE(node4.outputs.contains(7));
			EXPECT_FALSE(node3.inputs.contains(11));
			EXPECT_TRUE(node4.outputs.end() == node4.outputs.find(7));
		}
		template<typename N, typename... Args>
		void test_move_destruction(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7);
			auto node4_ptr = new node_type(graph1_ptr, 11);
			node1.add_input(3, args...);
			node1.add_input(5, args...);
			node1.add_input(7, args...);
			node1.add_output(7, args...);
			node4_ptr->mirror(node1); //changed from clone_links, may need to adjust tests

			node_type node5 = std::move(node1);
			EXPECT_EQ(3, node5.ID());
			EXPECT_TRUE(graph1_ptr->check(3, &node5));
			EXPECT_FALSE(node1.is_managed());
			EXPECT_EQ(0, node1.inputs.size() + node1.outputs.size());
			EXPECT_TRUE(node5.inputs.contains(3));
			EXPECT_TRUE(node5.outputs.contains(3));
			EXPECT_TRUE(node5.inputs.contains(5));
			EXPECT_TRUE(node5.inputs.contains(7));
			EXPECT_TRUE(node5.outputs.contains(7));
			EXPECT_EQ(5, node5.inputs.size() + node5.outputs.size());

			node1 = std::move(node5);
			EXPECT_TRUE(graph1_ptr->check(3, &node1));
			EXPECT_FALSE(node5.is_managed());
			EXPECT_EQ(0, node5.inputs.size() + node5.outputs.size());
			EXPECT_TRUE(node1.inputs.contains(3));
			EXPECT_TRUE(node1.outputs.contains(3));
			EXPECT_TRUE(node1.inputs.contains(5));
			EXPECT_TRUE(node1.inputs.contains(7));
			EXPECT_TRUE(node1.outputs.contains(7));
			EXPECT_EQ(5, node1.inputs.size() + node1.outputs.size());

			delete node4_ptr;
			node4_ptr = nullptr;
			EXPECT_FALSE(graph1_ptr->manages(11));
			EXPECT_FALSE(node1.inputs.contains(11));
			EXPECT_FALSE(node1.outputs.contains(11));
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
		test_construction<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Add_Remove) {
		typedef ben::stdDirectedNode<double> node_type;
		test_add_remove<node_type>(3.14159);
	}
	TEST_F(DirectedNodes, value_Node_Iteration) {
		typedef ben::stdDirectedNode<double> node_type;
		test_iteration<node_type>(3.14159);
	}
	TEST_F(DirectedNodes, value_Node_Move_Destruction) {
		typedef ben::stdDirectedNode<double> node_type;
		test_move_destruction<node_type>(3.14159);
	}


	class UndirectedNodes : public ::testing::Test {
	//this is nearly identical to the DirectedNode test set
	protected:
		template<typename N>
		void test_construction() {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();
			
			auto node1_ptr = new node_type(graph1_ptr);
			EXPECT_EQ(0, node1_ptr->size());
			EXPECT_TRUE(graph1_ptr->check(node1_ptr->ID(), node1_ptr));

			auto node2_ptr = new node_type(graph1_ptr, 3);
			EXPECT_TRUE(graph1_ptr->check(3, node2_ptr));

			auto node3_ptr = new node_type();
			EXPECT_FALSE(node3_ptr->is_managed());
			auto node4_ptr = new node_type(7);
			EXPECT_FALSE(node4_ptr->is_managed());
		}
		template<typename N, typename... Args>
		void test_add_remove(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7), node4(graph1_ptr, 11);
			EXPECT_TRUE(node1.add(3, args...)); //creates Path 3-3
			EXPECT_TRUE(node1.contains(3));
			EXPECT_TRUE(node1.add(5, args...)); //creates Path 3-5
			EXPECT_TRUE(node1.contains(5));
			EXPECT_TRUE(node2.contains(3));
			EXPECT_FALSE(node1.add(5, args...)); 
			EXPECT_TRUE(node1.add(7, args...)); //creates Path 3-7
			EXPECT_TRUE(node1.contains(7));
			EXPECT_TRUE(node3.contains(3));
			EXPECT_FALSE(node1.contains(11));
			EXPECT_TRUE(node2.add(11, args...)); //creates Path 5-11
			EXPECT_FALSE(node3.contains(5));
			EXPECT_FALSE(node3.contains(7));
			EXPECT_FALSE(node3.contains(11));

			EXPECT_EQ(1, node4.size());
			EXPECT_EQ(2, node2.size());
			EXPECT_TRUE(node4.mirror(node2));//should have 3, 5
			EXPECT_EQ(2, node4.size());
			EXPECT_TRUE(node4.contains(3)); 
			EXPECT_TRUE(node4.contains(5));
			EXPECT_FALSE(node4.contains(7));
			EXPECT_FALSE(node4.contains(11)); 
			EXPECT_TRUE(node1.contains(11));
			EXPECT_TRUE(node2.contains(11));
			
			node_type node5(13);
			EXPECT_FALSE(node5.mirror(node1));
			EXPECT_FALSE(node5.contains(3));

			EXPECT_TRUE(node5.join_index(graph1_ptr));

			node4.remove(7);
			EXPECT_FALSE(node4.contains(7));
			EXPECT_FALSE(node3.contains(11));
			node4.remove(7);

			node1.clear();
			EXPECT_FALSE(node1.contains(3));
			EXPECT_FALSE(node1.contains(5));
			EXPECT_FALSE(node1.contains(7));

			//testing ownership semantics of graph
			graph1_ptr.reset();
			EXPECT_EQ(0, node1.size());
			EXPECT_EQ(1, node2.size());
			EXPECT_EQ(0, node3.size());
			EXPECT_EQ(1, node4.size());
			EXPECT_TRUE(node1.is_managed());
			EXPECT_TRUE(node2.is_managed());
			EXPECT_TRUE(node3.is_managed());
			EXPECT_TRUE(node4.is_managed());
		}
		template<typename N, typename... Args>
		void test_iteration(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7), node4(graph1_ptr, 11);
			node1.add(3, args...);
			node1.add(5, args...);
			node1.add(7, args...);
			node4.mirror(node1); //changed from clone_links, may need to adjust tests
			
			for(auto iter = node1.begin(); iter != node1.end(); ++iter) {
				EXPECT_TRUE(graph1_ptr->elem(iter->get_address()).contains(3));
				EXPECT_EQ(iter->get_address(), node1.walk(iter).ID());
			}

			node4.remove(node4.find(5));
			EXPECT_FALSE(node4.contains(5));
			EXPECT_FALSE(node2.contains(11));
			EXPECT_TRUE(node4.end() == node4.find(5));
		}
		template<typename N, typename... Args> 
		void test_move_destruction(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7);
			auto node4_ptr = new node_type(graph1_ptr, 11);
			node1.add(3, args...);
			node1.add(5, args...);
			node1.add(7, args...);
			node4_ptr->clone_links(node1); 

			node_type node5 = std::move(node1);
			EXPECT_EQ(3, node5.ID());
			EXPECT_TRUE(graph1_ptr->check(3, &node5));
			EXPECT_FALSE(node1.is_managed());
			EXPECT_EQ(0, node1.size());
			EXPECT_TRUE(node5.contains(3));
			EXPECT_TRUE(node5.contains(5));
			EXPECT_TRUE(node5.contains(7));
			EXPECT_EQ(4, node5.size());

			node1 = std::move(node5);
			EXPECT_TRUE(graph1_ptr->check(3, &node1));
			EXPECT_FALSE(node5.is_managed());
			EXPECT_EQ(0, node5.size());
			EXPECT_TRUE(node1.contains(3));
			EXPECT_TRUE(node1.contains(5));
			EXPECT_TRUE(node1.contains(7));
			EXPECT_EQ(4, node1.size());

			delete node4_ptr;
			node4_ptr = nullptr;
			EXPECT_FALSE(graph1_ptr->contains(11));
			EXPECT_FALSE(node1.contains(11));
		}		
	};

	TEST_F(UndirectedNodes, value_Node_Construction) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		test_construction<node_type>();
	}
	TEST_F(UndirectedNodes, value_Node_Add_Remove) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		test_add_remove<node_type>(3.14159);
	}
	TEST_F(UndirectedNodes, value_Node_Iteration) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		//test_iteration<node_type>(3.14159);
	}
	TEST_F(UndirectedNodes, value_Node_Move_Destruction) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		//test_move_destruction<node_type>(3.14159);
	}


	class Graphs : public ::testing::Test {
	protected:
		template<typename N> 
		void test_add_remove() {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();
			auto graph2_ptr = std::make_shared<graph_type>();
		
			EXPECT_EQ(1, graph1_ptr.use_count());
			EXPECT_EQ(1, graph2_ptr.use_count());

			auto node1_ptr = new node_type(graph1_ptr, 3);
			auto node2_ptr = new node_type(5);
			auto node3_ptr = new node_type(7);
			
			EXPECT_TRUE(graph1_ptr->check(3, node1_ptr));
			EXPECT_FALSE(graph1_ptr->check(5, node2_ptr));
			EXPECT_TRUE(node2_ptr->join_index(graph1_ptr));

			EXPECT_EQ(3, graph1_ptr.use_count());
			EXPECT_EQ(1, graph2_ptr.use_count());

			EXPECT_TRUE(graph1_ptr->check(5, node2_ptr));
			EXPECT_TRUE(node3_ptr->join_index(graph1_ptr));

			EXPECT_EQ(0, graph2_ptr->size());
			EXPECT_TRUE(node2_ptr->join_index(graph2_ptr));
			
			EXPECT_EQ(3, graph1_ptr.use_count());
			EXPECT_EQ(2, graph2_ptr.use_count());

			EXPECT_TRUE(graph2_ptr->check(5, node2_ptr));
			EXPECT_EQ(1, graph2_ptr->size());
			EXPECT_EQ(2, graph1_ptr->size());

			node2_ptr->leave_index();
			EXPECT_EQ(0, graph2_ptr->size());
			EXPECT_FALSE(node2_ptr->is_managed());
			
			EXPECT_TRUE(node2_ptr->join_index(graph2_ptr));
			EXPECT_EQ(3, graph1_ptr.use_count());
			EXPECT_EQ(2, graph2_ptr.use_count());
			EXPECT_TRUE(node2_ptr->is_managed());

			delete node1_ptr; node1_ptr = nullptr;
			EXPECT_EQ(2, graph1_ptr.use_count());

			delete node2_ptr; node2_ptr = nullptr;
			EXPECT_EQ(1, graph2_ptr.use_count());

			delete node3_ptr; node3_ptr = nullptr;
			EXPECT_EQ(1, graph1_ptr.use_count()); 
		}
		template<typename N>
		void test_merge_move_destruction() {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();
			auto graph2_ptr = std::make_shared<graph_type>();
			
			auto node1_ptr = new node_type(graph1_ptr, 3);
			auto node2_ptr = new node_type(graph1_ptr, 5);
			auto node3_ptr = new node_type(graph2_ptr, 7);
			auto node4_ptr = new node_type(graph2_ptr, 11);

			EXPECT_TRUE(merge(graph1_ptr, graph2_ptr));
			EXPECT_EQ(4, graph1_ptr->size());
			EXPECT_EQ(0, graph2_ptr->size());
			EXPECT_EQ(graph1_ptr, node1_ptr->get_index());
			EXPECT_EQ(graph1_ptr, node2_ptr->get_index());

			graph1_ptr.reset();
			graph2_ptr.reset();
			EXPECT_TRUE(node1_ptr->is_managed());
			EXPECT_TRUE(node2_ptr->is_managed());
			EXPECT_TRUE(node3_ptr->is_managed());

			delete node1_ptr; node1_ptr = nullptr;
			delete node2_ptr; node2_ptr = nullptr;
			delete node3_ptr; node3_ptr = nullptr;
			delete node4_ptr; node4_ptr = nullptr;
		}
		template<typename N>
		void test_content() {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();
			auto graph2_ptr = std::make_shared<graph_type>();
			
			auto node1_ptr = new node_type(graph1_ptr, 3);
			auto node2_ptr = new node_type(graph1_ptr, 5);
			auto node3_ptr = new node_type(graph1_ptr, 7);

			for(node_type& x : *graph1_ptr) {
				EXPECT_TRUE(graph1_ptr->check(x.ID(), &x));
			}
			
			EXPECT_TRUE(graph1_ptr->manages(7));
			EXPECT_FALSE(graph2_ptr->manages(7));

			EXPECT_TRUE(graph2_ptr->end() == graph2_ptr->find(3));
			auto iter = graph1_ptr->begin();
			while(iter->ID() != 7) ++iter;
			EXPECT_TRUE(iter == graph1_ptr->find(7));
			EXPECT_TRUE(graph1_ptr->end() == graph1_ptr->find(4));

			delete node1_ptr, node2_ptr, node3_ptr;
		}
	};

	TEST_F(Graphs, DirectedNode_Add_Remove) {
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		test_add_remove<node_type>();		
	}
	TEST_F(Graphs, DirectedNode_Merge_Move_Destruction) {		
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		test_merge_move_destruction<node_type>();
	}
	TEST_F(Graphs, DirectedNode_Content) {
		using namespace ben;
		typedef stdMessageNode<double, 1> node_type;
		test_content<node_type>();
	}
	TEST_F(Graphs, UndirectedNode_Add_Remove) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		test_add_remove<node_type>();
	}
	TEST_F(Graphs, UndirectedNode_Merge_Move_Destruction) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		test_merge_move_destruction<node_type>();
	}
	TEST_F(Graphs, UndirectedNode_Content) {
		using namespace ben;
		typedef stdUndirectedNode<double> node_type;
		test_content<node_type>();
	}

} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

