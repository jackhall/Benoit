/*
	Benoit: a flexible framework for distributed graphs and spaces
	Copyright (C) 2011-2013  John Wendell Hall

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
//	make
//	./test_benoit

#include <iostream>
#include <vector>
#include <random>
#include <gtest/gtest.h>
#include "Port.h"
#include "Index.h"
#include "Graph.h"
#include "Singleton.h"
#include "Node.h"

namespace {

	template<typename S> 
	class DerivedIndex : public ben::Index<S> {
	private:
		typedef ben::Index<S> base_type;
		typedef DerivedIndex self_type;

		bool perform_add(ben::Singleton* ptr) { return true; }
		void perform_remove(ben::Singleton* ptr) {}
		bool perform_merge(base_type& other) { 
			return true;
		}

	public:
		DerivedIndex() = default;
		~DerivedIndex() = default;

		typedef S singleton_type;		
		typedef typename base_type::iterator iterator;
	};

	class DerivedSingleton : public ben::Singleton {
	private:
		void perform_leave() {}
	public:
		typedef DerivedIndex<DerivedSingleton> index_type;
		typedef ben::Singleton base_type;

		DerivedSingleton() : Singleton() {}
		DerivedSingleton(const id_type id) : Singleton(id) {}
		DerivedSingleton(std::shared_ptr<index_type> x) : Singleton(x) {}
		DerivedSingleton(std::shared_ptr<index_type> x, const id_type id) : Singleton(x, id) {}
		DerivedSingleton(const DerivedSingleton& rhs) = delete;
		DerivedSingleton(DerivedSingleton&& rhs) : Singleton( std::move(rhs) ) {}
		DerivedSingleton& operator=(const DerivedSingleton& rhs) = delete;
		DerivedSingleton& operator=(DerivedSingleton&& rhs) { Singleton::operator=( std::move(rhs) ); }; 
		~DerivedSingleton() = default;
		
		bool join_index(std::shared_ptr<index_type> x) { return base_type::join_index(x); }
		std::shared_ptr<index_type> get_index() const { return std::static_pointer_cast<index_type>(base_type::get_index()); }	
	};

	TEST(IndexSingleton, Construction) {
		using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef DerivedIndex<singleton_type> index_type;
		
		//======= Index<> default constructor =========
		auto index1_ptr = std::make_shared<index_type>();
			EXPECT_EQ(0, index1_ptr->size());
		
		//======= Singleton index constructor ========
		singleton_type singleton1(index1_ptr);
			//singleton1 tests
			EXPECT_EQ(1000, singleton1.ID());
			EXPECT_TRUE(singleton1.is_managed());
			EXPECT_EQ(index1_ptr, singleton1.get_index());
			//index1 tests
			EXPECT_EQ(1, index1_ptr->size());
			//both
			EXPECT_TRUE( index1_ptr->manages(singleton1.ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton1.ID(), &singleton1) );
		
		//====== Index<> destructor ========= //unnecessary; managed Nodes now have shared ownership
		//
		//====== Singleton index constructor ====== 
		singleton_type* singleton2_ptr = new singleton_type(index1_ptr);
			//singleton2 tests
			EXPECT_EQ(1001, singleton2_ptr->ID());
			EXPECT_TRUE(singleton2_ptr->is_managed());
			EXPECT_EQ(index1_ptr, singleton2_ptr->get_index());
			//index2 tests
			EXPECT_EQ(2, index1_ptr->size());
			//both
			EXPECT_TRUE( index1_ptr->manages(singleton2_ptr->ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton2_ptr->ID(), singleton2_ptr) );
		
		//====== Singleton destructor =======
		unsigned short id2 = singleton2_ptr->ID();
		delete singleton2_ptr;
		singleton2_ptr = nullptr;
			//index1_ptr->tests
			EXPECT_EQ(1, index1_ptr->size());
			EXPECT_FALSE(index1_ptr->manages(id2));
		
		//====== Singleton default constructor ======
		singleton_type singleton3;
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.is_managed());
		
		//====== Singleton id constructor ======
		singleton_type singleton4(5000);
			EXPECT_EQ(5000, singleton4.ID());
			EXPECT_FALSE(singleton4.is_managed());
		
		//====== Singleton index+id constructor ======
		singleton_type singleton5(index1_ptr, 6000);
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.is_managed());
			EXPECT_EQ(index1_ptr, singleton5.get_index());
			//index1_ptr->tests
			EXPECT_EQ(2, index1_ptr->size());
			//both
			EXPECT_TRUE( index1_ptr->manages(singleton5.ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton5.ID(), &singleton5) );
		
		//====== Singleton move constructor ======
		//---- from is_managed
		singleton_type singleton6( std::move(singleton5) );
			//singleton6 tests
			EXPECT_EQ(6000, singleton6.ID());
			EXPECT_TRUE(singleton6.is_managed());
			EXPECT_EQ(index1_ptr, singleton6.get_index());
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_FALSE(singleton5.is_managed());
			EXPECT_FALSE(singleton5.get_index());
			//index1_ptr tests
			EXPECT_EQ(2, index1_ptr->size());
			EXPECT_TRUE( index1_ptr->manages(singleton6.ID()) );
			//singleton6+index1_ptr tests
			EXPECT_TRUE( index1_ptr->check(singleton6.ID(), &singleton6) );
			//singleton5+index1_ptr tests
			EXPECT_FALSE( index1_ptr->check(singleton5.ID(), &singleton5) );
		
		//---- from null
		singleton_type singleton7( std::move(singleton3) ); 
			//singleton7 tests
			EXPECT_EQ(1002, singleton7.ID()); 
			EXPECT_FALSE(singleton7.is_managed());
			//singleton3 tests
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.is_managed());
		
		//====== Singleton move assignment operator ======
		//---- is_managed -> null
		singleton5 = std::move(singleton6);
			//singleton6 tests
			EXPECT_EQ(6000, singleton6.ID());
			EXPECT_FALSE(singleton6.is_managed());
			EXPECT_NE(index1_ptr, singleton6.get_index());
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.is_managed());
			EXPECT_EQ(index1_ptr, singleton5.get_index());
			//index1_ptr->tests
			EXPECT_EQ(2, index1_ptr->size());
			EXPECT_TRUE( index1_ptr->manages(singleton6.ID()) );
			//singleton6+index1 tests
			EXPECT_FALSE( index1_ptr->check(singleton6.ID(), &singleton6) );
			//singleton5+index1 tests
			EXPECT_TRUE( index1_ptr->check(singleton5.ID(), &singleton5) );
		
		//---- null -> null
		singleton3 = std::move(singleton7);
			//singleton7 tests
			EXPECT_EQ(1002, singleton7.ID());
			EXPECT_FALSE(singleton7.is_managed());
			//singleton3 tests
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.is_managed());
		
		//---- null -> is_managed
		singleton5 = std::move(singleton4);
			//singleton3 tests
			EXPECT_EQ(5000, singleton5.ID());
			EXPECT_FALSE(singleton5.is_managed());
			//singleton4 tests
			EXPECT_EQ(5000, singleton4.ID());
			EXPECT_FALSE(singleton4.is_managed());
			//index1 tests
			EXPECT_EQ(1, index1_ptr->size());
			EXPECT_FALSE( index1_ptr->manages(singleton5.ID()) );
		
		//---- is_managed -> is_managed
		//prepare new singleton
		auto index2_ptr = std::make_shared<index_type>();
		singleton_type singleton8(index1_ptr); 
		singleton_type singleton9(index2_ptr);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_TRUE(singleton8.is_managed());
			EXPECT_EQ(index1_ptr, singleton8.get_index());
			//singleton9 tests
			EXPECT_EQ(1004, singleton9.ID());
			EXPECT_TRUE(singleton9.is_managed());
			EXPECT_EQ(index2_ptr, singleton9.get_index());
			//index2 tests
			EXPECT_EQ(1, index2_ptr->size()); 
			//both
			EXPECT_TRUE( index1_ptr->manages(singleton8.ID()) ); 
			EXPECT_TRUE( index2_ptr->manages(singleton9.ID()) ); 
			EXPECT_TRUE( index1_ptr->check(singleton8.ID(), &singleton8) ); 
			EXPECT_TRUE( index2_ptr->check(singleton9.ID(), &singleton9) ); 
		
		//actual move
		singleton9 = std::move(singleton8);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_FALSE(singleton8.is_managed());
			//singleton9 tests
			EXPECT_EQ(1003, singleton9.ID());
			EXPECT_TRUE(singleton9.is_managed());
			EXPECT_EQ(index1_ptr, singleton9.get_index());
			//index2 tests
			EXPECT_EQ(0, index2_ptr->size());
			EXPECT_FALSE( index2_ptr->manages(singleton9.ID()) );
	}
	
	TEST(IndexSingleton, GlobalMethods) {
		using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef DerivedIndex<singleton_type> index_type;

		auto index1_ptr = std::make_shared<index_type>();
		singleton_type singleton1(index1_ptr), singleton2(index1_ptr), singleton3(index1_ptr, 5000);
		auto index2_ptr = std::make_shared<index_type>();
		singleton_type singleton4(index2_ptr), singleton5(index2_ptr, 5003), singleton6(index2_ptr, 5000);
		
		//initial checks
		EXPECT_EQ(3, index1_ptr->size());
		EXPECT_EQ(3, index2_ptr->size());
		EXPECT_EQ(index1_ptr, singleton1.get_index());
		EXPECT_EQ(index1_ptr, singleton2.get_index());
		EXPECT_EQ(index1_ptr, singleton3.get_index());
		EXPECT_EQ(index2_ptr, singleton4.get_index());
		EXPECT_EQ(index2_ptr, singleton5.get_index());
		EXPECT_EQ(index2_ptr, singleton6.get_index());
		
		//find tests
        EXPECT_FALSE(index1_ptr->look_up(31415));
        EXPECT_EQ(&singleton3, index1_ptr->look_up(5000));
        EXPECT_EQ(&singleton6, index2_ptr->look_up(5000));
				
		//add test
		DerivedSingleton singleton7(5002);
		//null -> is_managed
		EXPECT_TRUE(singleton7.join_index(index1_ptr));
		EXPECT_TRUE(index1_ptr->check(5002, &singleton7));
		EXPECT_EQ(index1_ptr, singleton7.get_index());
		
		//is_managed -> is_managed redundant
		EXPECT_TRUE(singleton6.join_index(index1_ptr));
		EXPECT_EQ(5, index1_ptr->size());
		EXPECT_TRUE(index1_ptr->check(5000, &singleton3));
		EXPECT_TRUE(index1_ptr->check(singleton6.ID(), &singleton6));
		EXPECT_FALSE(index2_ptr->manages(5000));
		EXPECT_EQ(2, index2_ptr->size());
		EXPECT_EQ(index1_ptr, singleton6.get_index());
		
		//is_managed -> managed
		EXPECT_TRUE(singleton5.join_index(index1_ptr));
		EXPECT_TRUE(index1_ptr->check(5003, &singleton5));
		EXPECT_FALSE(index2_ptr->manages(5003));
		EXPECT_EQ(index1_ptr, singleton5.get_index());	
		
		//merge (redundant for id=5000)
		singleton_type singleton8(index2_ptr, 5000);
		EXPECT_FALSE(merge(index2_ptr, index1_ptr));
			//index1 tests
			EXPECT_EQ(6, index1_ptr->size());
			EXPECT_TRUE(index1_ptr->check(singleton1.ID(), &singleton1));
			EXPECT_EQ(index1_ptr, singleton1.get_index());
			EXPECT_TRUE(index1_ptr->check(singleton2.ID(), &singleton2));
			EXPECT_EQ(index1_ptr, singleton2.get_index());
			EXPECT_TRUE(index1_ptr->check(5000, &singleton3));
			EXPECT_EQ(index1_ptr, singleton3.get_index());
			EXPECT_TRUE(index1_ptr->check(5003, &singleton5));
			EXPECT_EQ(index1_ptr, singleton5.get_index());
			EXPECT_TRUE(index1_ptr->check(5002, &singleton7));
			EXPECT_EQ(index1_ptr, singleton6.get_index());
			EXPECT_TRUE(index1_ptr->check(singleton6.ID(), &singleton6));
			EXPECT_EQ(index1_ptr, singleton7.get_index());
			//index2 tests
			EXPECT_EQ(2, index2_ptr->size());
			EXPECT_TRUE(index2_ptr->check(singleton4.ID(), &singleton4));
			EXPECT_EQ(index2_ptr, singleton4.get_index());
			EXPECT_TRUE(index2_ptr->check(5000, &singleton8));
			EXPECT_EQ(index2_ptr, singleton8.get_index());
		
		//remove
		singleton3.leave_index(); 
		EXPECT_FALSE(singleton3.is_managed()); 
		EXPECT_FALSE(index1_ptr->manages(5000));
		
		//merge_into (valid)
		EXPECT_TRUE(merge(index2_ptr, index1_ptr)); 
			//index1 tests 
			EXPECT_EQ(0, index1_ptr->size());
			//index2 tests
			EXPECT_EQ(7, index2_ptr->size());
			EXPECT_TRUE(index2_ptr->check(singleton1.ID(), &singleton1));
			EXPECT_EQ(index2_ptr, singleton1.get_index());
			EXPECT_TRUE(index2_ptr->check(singleton2.ID(), &singleton2));
			EXPECT_EQ(index2_ptr, singleton2.get_index());
			EXPECT_TRUE(index2_ptr->check(singleton4.ID(), &singleton4));
			EXPECT_EQ(index2_ptr, singleton4.get_index());
			EXPECT_TRUE(index2_ptr->check(5003, &singleton5));
			EXPECT_EQ(index2_ptr, singleton5.get_index());
			EXPECT_TRUE(index2_ptr->check(singleton6.ID(), &singleton6));
			EXPECT_EQ(index2_ptr, singleton6.get_index());
			EXPECT_TRUE(index2_ptr->check(5002, &singleton7));
			EXPECT_EQ(index2_ptr, singleton7.get_index());
			EXPECT_TRUE(index2_ptr->check(5000, &singleton8));
			EXPECT_EQ(index2_ptr, singleton8.get_index());
	}

    class Buffers : public ::testing::Test {
        template<typename SIGNAL>
        void test_message(SIGNAL received, SIGNAL sent) {
            using namespace ben;
            Buffer<SIGNAL, 1> link;
            EXPECT_FALSE(link.pop(received));
            EXPECT_TRUE(link.push(send));
            EXPECT_TRUE(link.pop(received));
            EXPECT_EQ(send, received);
        }
    };

	TEST_F(Buffers, DoubleInstance) {
		using namespace ben;
        std::default_random_engine gen;
        std::uniform_real_distribution<double> random_real(0.0, 1.0);
        double received, sent = random_real(gen);
        test_message(received, sent);
	}
    TEST(Buffers, VectorInstance) {
		using namespace ben;
        std::default_random_engine gen;
        std::uniform_real_distribution<double> random_real(0.0, 1.0);
        std::vector<double> received, sent = {random_real(gen), random_real(gen)};
        test_message(received, sent);
    }
    TEST(Buffers, ArrayInstance) {
		using namespace ben;
        std::default_random_engine gen;
        std::uniform_real_distribution<double> random_real(0.0, 1.0);
        std::array<double, 2> received, sent = {random_real(gen), random_real(gen)};
        test_message(received, sent);
    }
    TEST(Buffers, FunctionInstance) {
		using namespace ben;
        std::function<double()> received, sent = []() { return 3.14159 };
        test_message(received, sent);
    }


	class Port : public ::testing::Test {
	protected:
		template<typename IN, typename OUT, typename... ARGS> 
		void test_normal_construction(ARGS... args) { 
			using namespace ben;
			typedef IN input_type;
			typedef OUT output_type;
	
			//normal construction
			auto input_port_ptr = new input_type(5, args...);
			EXPECT_EQ(5, input_port_ptr->get_address());
			EXPECT_TRUE(input_port_ptr->is_ghost());
			
			//complement constructors
			auto output_port_ptr = new output_type(*input_port_ptr, 19);
			EXPECT_EQ(19, output_port_ptr->get_address());
			EXPECT_FALSE(input_port_ptr->is_ghost());
			EXPECT_FALSE(output_port_ptr->is_ghost());
		
			//copy constructor
			auto input_port_ptr2 = new input_type(*input_port_ptr);
			EXPECT_EQ(5, input_port_ptr2->get_address());
			EXPECT_FALSE(input_port_ptr2->is_ghost());
			EXPECT_FALSE(input_port_ptr->is_ghost());

			output_type output_port2(*output_port_ptr);
			EXPECT_EQ(11, output_port2.get_address());
			EXPECT_FALSE(output_port2.is_ghost());

			//destruction
			delete input_port_ptr; input_port_ptr = nullptr;
			EXPECT_FALSE(output_port_ptr->is_ghost());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_FALSE(input_port_ptr2->is_ghost());
            
            //delete input_port_ptr2; input_port_ptr2 = nullptr;
            //EXPECT_FALSE(output_port_ptr->is_ghost());
            //EXPECT_FALSE(output_port2.is_ghost());

            //delete output_port_ptr; output_port_ptr = nullptr;
            //EXPECT_TRUE(output_port2.is_ghost());

			//move constructor
			input_type input_port3(std::move(*input_port_ptr2));
			EXPECT_EQ(17, input_port3.get_address());
			EXPECT_FALSE(input_port3.is_ghost());
			EXPECT_TRUE(input_port_ptr2->is_ghost()); 

			output_type output_port3(std::move(output_port2));
			EXPECT_EQ(19, output_port3.get_address());
			EXPECT_FALSE(output_port3.is_ghost());
			EXPECT_TRUE(output_port2.is_ghost()); 

			//move assignment
			*input_port_ptr2 = std::move(input_port3);
			EXPECT_EQ(17, input_port_ptr2->get_address());
			EXPECT_FALSE(input_port_ptr2->is_ghost());
			EXPECT_TRUE(input_port3.is_ghost()); 

			output_port2 = std::move(output_port3);
			EXPECT_EQ(19, output_port2.get_address());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_TRUE(output_port3.is_ghost()); //fails
		
			//assignment
			input_port3 = *input_port_ptr2;
			EXPECT_EQ(17, input_port3.get_address());
			EXPECT_FALSE(input_port_ptr2->is_ghost());
			EXPECT_FALSE(input_port3.is_ghost());

			output_port3 = output_port2;
			EXPECT_EQ(19, output_port3.get_address());
			EXPECT_FALSE(output_port2.is_ghost());
			EXPECT_FALSE(output_port3.is_ghost());
		}	
	    template<typename IN>
        void test_complement_construction(const IN& input_port) {}
        template<typename PORT>
        void test_copy_and_assign(const PORT& port) {}
        template<typename IN, typename OUT>
        void test_destruction(IN input_port, OUT output_port) {}
        template<typename IN, typename OUT, typename GENERATOR>
        void test_data_passing(IN input_port, OUT output_port, GENERATOR gen) {}
    };

	TEST_F(Port, Port_Construction) {
		SCOPED_TRACE("Ports");
		typedef ben::InPort<ben::Buffer<double, 2>>  input_type;
		typedef ben::OutPort<ben::Buffer<double, 2>> output_type;
		test_construction<input_type, output_type>();
	}
	TEST_F(Port, Path_Construction) {
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

		EXPECT_FALSE(input_port.pop(test_signal));
		EXPECT_TRUE(output_port.push(signal1));
		EXPECT_FALSE(input_port.pop(test_signal));
		EXPECT_TRUE(output_port.push(signal2));
		EXPECT_FALSE(output_port.push(signal3));
		EXPECT_TRUE(input_port.pop(test_signal));
		EXPECT_EQ(signal2, test_signal);
	}


	class Nodes : public ::testing::Test {
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
			EXPECT_TRUE(node1.link(3, args...)); //creates Port/Path
			EXPECT_TRUE(node1.linked_to(3));
			EXPECT_TRUE(node1.outputs.contains(3));
			EXPECT_TRUE(node1.link(5, args...)); //creates Port/Path
			EXPECT_TRUE(node1.linked_to(5));
			EXPECT_TRUE(node2.outputs.contains(3));
			EXPECT_FALSE(node1.link(5, args...)); //creates Port/Path
			EXPECT_TRUE(node1.add_output(7, args...)); //creates Port/Path
			EXPECT_TRUE(node1.outputs.contains(7));
			EXPECT_TRUE(node3.linked_to(3));
			EXPECT_FALSE(node1.linked_to(11)); 
			EXPECT_FALSE(node1.outputs.contains(5)); 
			
			EXPECT_TRUE(node4.mirror(node1)); //clone_links cannot be well-defined for DirectedNode
			EXPECT_FALSE(node4.linked_to(3));
			EXPECT_FALSE(node1.outputs.contains(11));
			EXPECT_FALSE(node4.outputs.contains(3));
			EXPECT_FALSE(node1.linked_to(11));
			EXPECT_TRUE(node4.linked_to(5));
			EXPECT_TRUE(node2.outputs.contains(11));
			EXPECT_TRUE(node4.outputs.contains(7));
			EXPECT_TRUE(node4.linked_to(11));
			EXPECT_TRUE(node4.outputs.contains(11));
			EXPECT_FALSE(node4.outputs.contains(5));
			
			node_type node5(13);
			EXPECT_FALSE(node5.mirror(node1));
			EXPECT_FALSE(node5.linked_to(3));

			node4.unlink(7);
			EXPECT_FALSE(node4.linked_to(7));
			EXPECT_FALSE(node3.outputs.contains(11));
			node4.remove_output(7);
			EXPECT_FALSE(node4.outputs.contains(7));	
			EXPECT_FALSE(node3.linked_to(11));	

			node1.clear();
			EXPECT_FALSE(node1.linked_to(3));
			EXPECT_FALSE(node1.linked_to(5));
			EXPECT_FALSE(node1.linked_to(7));
			EXPECT_FALSE(node1.outputs.contains(7));

			//testing ownership semantics of graph
			graph1_ptr.reset();
			EXPECT_EQ(0, node1.size() + node1.outputs.size());
			EXPECT_EQ(1, node2.size() + node2.outputs.size());
			EXPECT_EQ(0, node3.size() + node3.outputs.size());
			EXPECT_EQ(3, node4.size() + node4.outputs.size());
			EXPECT_TRUE(node1.is_managed());
			EXPECT_TRUE(node2.is_managed());
			EXPECT_TRUE(node3.is_managed());
			EXPECT_TRUE(node4.is_managed());
		}
		template<typename N, typename... Args>
		void test_move_destruction(Args... args) {
			using namespace ben;
			typedef N node_type;
			typedef Graph<node_type> graph_type;
			auto graph1_ptr = std::make_shared<graph_type>();

			node_type node1(graph1_ptr, 3), node2(graph1_ptr, 5), node3(graph1_ptr, 7);
			auto node4_ptr = new node_type(graph1_ptr, 11);
			node1.link(3, args...);
			node1.link(5, args...);
			node1.link(7, args...);
			node1.add_output(7, args...);
			node4_ptr->mirror(node1); //changed from clone_links, may need to adjust tests

			node_type node5 = std::move(node1);
			EXPECT_EQ(3, node5.ID());
			EXPECT_TRUE(graph1_ptr->check(3, &node5));
			EXPECT_FALSE(node1.is_managed());
			EXPECT_EQ(0, node1.size() + node1.outputs.size());
			EXPECT_TRUE(node5.linked_to(3));
			EXPECT_TRUE(node5.outputs.contains(3));
			EXPECT_TRUE(node5.linked_to(5));
			EXPECT_TRUE(node5.linked_to(7));
			EXPECT_TRUE(node5.outputs.contains(7));
			EXPECT_EQ(5, node5.size() + node5.outputs.size());

			node1 = std::move(node5);
			EXPECT_TRUE(graph1_ptr->check(3, &node1));
			EXPECT_FALSE(node5.is_managed());
			EXPECT_EQ(0, node5.size() + node5.outputs.size());
			EXPECT_TRUE(node1.linked_to(3));
			EXPECT_TRUE(node1.outputs.contains(3));
			EXPECT_TRUE(node1.linked_to(5));
			EXPECT_TRUE(node1.linked_to(7));
			EXPECT_TRUE(node1.outputs.contains(7));
			EXPECT_EQ(5, node1.size() + node1.outputs.size());

			delete node4_ptr;
			node4_ptr = nullptr;
			EXPECT_FALSE(graph1_ptr->manages(11));
			EXPECT_FALSE(node1.linked_to(11));
			EXPECT_FALSE(node1.outputs.contains(11));
		}
	};

	TEST_F(Nodes, MessageNode_Construction) {
		typedef ben::MessageNode<double, 1> node_type;
		test_construction<node_type>();
	}
	TEST_F(DirectedNodes, MessageNode_Add_Remove) {
		typedef ben::MessageNode<double, 1> node_type;
		test_add_remove<node_type>();
	}
	TEST_F(DirectedNodes, MessageNode_Move_Destruction) {
		typedef ben::MessageNode<double, 1> node_type;
		test_move_destruction<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Construction) {
		typedef ben::DirectedNode<double> node_type;
		test_construction<node_type>();
	}
	TEST_F(DirectedNodes, value_Node_Add_Remove) {
		typedef ben::DirectedNode<double> node_type;
		test_add_remove<node_type>(3.14159);
	}
	TEST_F(DirectedNodes, value_Node_Move_Destruction) {
		typedef ben::DirectedNode<double> node_type;
		test_move_destruction<node_type>(3.14159);
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
	};

	TEST_F(Graphs, DirectedNode_Add_Remove) {
		using namespace ben;
		typedef MessageNode<double, 1> node_type;
		test_add_remove<node_type>();		
	}
	TEST_F(Graphs, DirectedNode_Merge_Move_Destruction) {		
		using namespace ben;
		typedef MessageNode<double, 1> node_type;
		test_merge_move_destruction<node_type>();
	}

} //anonymous namespace 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

