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
#include <memory>
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
///////////////////////////
////////////// Buffer tests
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
    TEST_F(Buffers, VectorInstance) {
		using namespace ben;
        std::default_random_engine gen;
        std::uniform_real_distribution<double> random_real(0.0, 1.0);
        std::vector<double> received, sent = {random_real(gen), random_real(gen)};
        test_message(received, sent);
    }
    TEST_F(Buffers, ArrayInstance) {
		using namespace ben;
        std::default_random_engine gen;
        std::uniform_real_distribution<double> random_real(0.0, 1.0);
        std::array<double, 2> received, sent = {random_real(gen), random_real(gen)};
        test_message(received, sent);
    }
    TEST_F(Buffers, FunctionInstance) {
		using namespace ben;
        std::function<double()> received, sent = []() { return 3.14159 };
        test_message(received, sent);
    }
////////////////////////////
//////////////// Port tests
	class Port : public ::testing::Test {
	protected:
		template<typename IN, typename... ARGS> 
		auto test_normal_construction(ARGS... args) { 
			std::unique_ptr<IN> input_port_ptr(new IN(5, args...));
			EXPECT_EQ(5, input_port_ptr->get_address());
			EXPECT_TRUE(input_port_ptr->is_ghost());
            return input_port_ptr;
		}	
	    template<typename IN>
        auto test_complement_construction(const IN& input_port, unsigned int ID) {
			std::unique_ptr<typename IN::complement_type> output_port_ptr(input_port, ID);
			EXPECT_EQ(ID, output_port_ptr->get_address());
			EXPECT_FALSE(input_port.is_ghost());
			EXPECT_FALSE(output_port_ptr->is_ghost());
            return output_port_ptr;
        }
        template<typename PORT>
        auto test_copy(const PORT& port) {
			std::unique_ptr<PORT> port_ptr(new PORT(port));
			EXPECT_EQ(port.get_address(), port_ptr->get_address());
			EXPECT_FALSE(port_ptr->is_ghost());
			EXPECT_FALSE(port.is_ghost());
            return port_ptr;
        }
        template<typename PORT>
        void test_assign(const PORT& port1, PORT& port2) {
            EXPECT_NE(port1.get_address(), port2.get_address());
			port2 = *port1;
			EXPECT_EQ(port1.get_address(), port2.get_address());
			EXPECT_FALSE(port1.is_ghost());
			EXPECT_FALSE(port2.is_ghost());
        }
        template<typename FIRST, typename... ARGS>
        void test_destruction(FIRST* port_ptr, ARGS... args) {
            //takes a set of ports that all point to the same link
            //all port objects will be deleted
            auto n = sizeof...(ARGS);
            if(n > 0) {
                EXPECT_FALSE(port_ptr->is_ghost());
			    delete port_ptr; port_ptr = nullptr;
                test_destruction(args...);
            } else {
                EXPECT_TRUE(port_ptr->is_ghost());
                delete port_ptr; port_ptr = nullptr;
            }
        }
        template<typename IN, typename OUT, typename SIGNAL>
        void test_data_passing(IN input_port, OUT output_port, SIGNAL sent) {
            EXPECT_FALSE(input_port.is_ghost());
            EXPECT_FALSE(output_port.is_ghost());
            EXPECT_TRUE(output_port.push(signal));
            decltype(sent) received;
            EXPECT_NE(sent, received);
            EXPECT_TRUE(input_port.pop(received));
            EXPECT_EQ(sent, received);
        }
    };

	TEST_F(Port, Standard) {
        using namespace ben;
        typedef Buffer<double,1> buffer_type;
		typedef InPort<buffer_type> input_port_type;
		typedef OutPort<buffer_type> output_port_type;
        typedef typename input_port_type::id_type id_type;

        //with original ports
        {
            SCOPED_TRACE("originals");
            //create first link (pair of ports)
            auto input_port_ptr = test_normal_construction<input_port_type, id_type>(1);
            auto output_port_ptr = test_complement_construction(*input_port_ptr, 2);

            //test message passing through this first pair
            std::default_random_engine gen;
            std::uniform_real_distribution<double> random_real(0.0, 1.0);
            test_data_passing(*input_port_ptr, *output_port_ptr, random_real());
        }
        //with copies
        {
            SCOPED_TRACE("copies");
            //copy each of the original ports and test message passing again
            auto input_port_ptr2 = test_copy(*input_port_ptr);
            auto output_port_ptr2 = test_copy(*output_port_ptr);
            test_data_passing(*input_port_ptr2, *output_port_ptr2* random_real()); 

            //test assignment and data passing
            auto input_port2_ptr = test_normal_construction<input_port_type, id_type>(3); 
            auto output_port2_ptr = test_complement_construction(*input_port2_ptr, 4);

            test_assign(*input_port_ptr, *input_port2_ptr);
            EXPECT_TRUE(output_port2_ptr->is_ghost());
            test_data_passing(*input_port2_ptr, *output_port_ptr, random_real());

            test_assign(*output_port_ptr, *output_port2_ptr);
            test_data_passing(*input_port_ptr, *output_port2_ptr, random_real());
        }
        
        test_destruction(input_port_ptr.release(), output_port_ptr.release(),
                         input_port_ptr2.release(), output_port_ptr2.release(),
                         input_port2_ptr.release(), output_port2_ptr.release());
	}
//////////////////////////
///////////////Node tests
	class Nodes : public ::testing::Test {
	protected:
        template<typename NODE>
        auto test_default_construction() {
            std::unique_ptr<NODE> node_ptr(new NODE());
            EXPECT_EQ(0, node_ptr->size());
            EXPECT_FALSE(node_ptr->get_index());
            return node_ptr;
        }
		template<typename NODE>
		auto test_graph_construction(std::shared_ptr< Graph<NODE> > graph_ptr, NODE::id_type id) {
			typedef Graph<NODE> graph_type;
			
			NODE node(graph_ptr);
			EXPECT_EQ(0, node.size());
			EXPECT_TRUE(graph_ptr->check(node.ID(), &node));
            EXPECT_EQ(node.get_index(), graph_ptr);

			std::unique_ptr<NODE> node_ptr(new NODE(graph_ptr, id));
			EXPECT_TRUE(graph_ptr->check(3, node_ptr));
			EXPECT_EQ(0, node_ptr->size());
			EXPECT_TRUE(graph_ptr->check(node_ptr->ID(), node_ptr));
            EXPECT_EQ(node_ptr->get_index(), graph_ptr);
            return node_ptr;
		}
        template<typename NODE>
        auto test_map_inputs(const NODE& node) {
            std::vector<typename NODE::id_type> link_addresses;
            node.for_inputs([&link_addresses] (const NODE::input_port_type& port) { 
                link_addresses.push_back(port.get_address()); } );
            EXPECT_EQ(node.size(), link_addresses.size());
            for(auto id : link_addresses) EXPECT_TRUE(node.linked_to(id));
            return link_addresses;
        }
		template<typename NODE, typename... Args>
		void test_link_unlink(NODE&, NODE& Args... args) {

		}
		template<typename NODE>
        void test_mirror(const NODE& node) {
            SCOPED_TRACE("mirror");
            NODE node2(node.get_index());
            auto link_addresses = test_map_inputs(node);
            node2.mirror(node);
            for(auto id : link_addresses) {
                EXPECT_TRUE(node.linked_to(id));
                EXPECT_TRUE(node2.linked_to(id));
            }
        }
        template<typename NODE>
        void test_move(NODE& source, NODE::id_type input_id, const NODE& output_node) {
            SCOPED_TRACE("move");
            //source should have an input link to "input_id" and an output link to "output_id" 
            EXPECT_TRUE(source.linked_to(input_id));
            auto node_id = source.ID();
            auto graph_ptr = source.get_index();
            EXPECT_TRUE(output_node.linked_to(node_id));

            NODE target(std::move(source));
            EXPECT_FALSE(source.get_index());
            EXPECT_FALSE(source.linked_to(input_id));
            EXPECT_EQ(node_id, target.ID());
            EXPECT_EQ(graph_ptr, target.get_index());
            EXPECT_TRUE(target.linked_to(input_id));
            EXPECT_TRUE(output_node.linked_to(node_id));
            //test data passing?

            source = std::move(target);
            EXPECT_FALSE(target.get_index());
            EXPECT_FALSE(target.linked_to(input_id));
            EXPECT_EQ(node_id, source.ID());
            EXPECT_EQ(graph_ptr, source.get_index());
            EXPECT_TRUE(source.linked_to(input_id));
            EXPECT_TRUE(output_node.linked_to(node_id));
            //test data passing?
        }
        template<typename FIRST, typename... ARGS>
		void test_destruction(std::vector<typename FIRST::id_type> deleted_ids, 
                              FIRST* node_ptr, ARGS... args) {
			auto graph_ptr = node_ptr->get_index();
            auto node_id = node_ptr->ID();
            EXPECT_TRUE(graph_ptr);
            if(graph_ptr) EXPECT_TRUE(graph_ptr->check(node_id, node_ptr));
            for(auto id : deleted_ids) EXPECT_FALSE(node_ptr->linked_to(id));

            delete node_ptr; node_ptr = nullptr;
            EXPECT_FALSE(graph_ptr->look_up(node_id));

            auto n = sizeof...(ARGS);
            if(n > 0) {
                deleted_ids.push_back(node_id);
                test_destruction(deleted_ids, args...);
            }
		}
	};

	TEST_F(Nodes, MessageNode_Construction) {
		typedef ben::MessageNode<double, 1> node_type;
		test_construction<node_type>();
	}
	TEST_F(Nodes, MessageNode_Add_Remove) {
		typedef ben::MessageNode<double, 1> node_type;
		test_add_remove<node_type>();
	}
	TEST_F(Nodes, MessageNode_Move_Destruction) {
		typedef ben::MessageNode<double, 1> node_type;
		test_move_destruction<node_type>();
	}
///////////////////////////
///////////////Graph tests
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
//////////////////////////

} 

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

