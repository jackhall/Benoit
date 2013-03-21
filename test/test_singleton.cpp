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
//	g++ -std=c++11 -g -I../src -I../Wayne/src -lgtest -lpthread test_singleton.cpp -o test_singleton
//	./test_singleton

#include <iostream>
#include "Index.h"
#include "Singleton.h"
#include "gtest/gtest.h"

namespace {

	template<typename S> 
	class DerivedIndex : public ben::Index<S> {
	private:
		typedef ben::Index<S> base_type;
		typedef DerivedIndex self_type;

		bool perform_add(ben::Singleton* ptr) { return true; }
		bool perform_remove(ben::Singleton* ptr) { return true; }
		bool perform_merge(base_type& other) { return true; }

	public:
		DerivedIndex() = default;
		~DerivedIndex() = default;

		typedef S singleton_type;		
		typedef typename base_type::iterator iterator;
	};

	struct DerivedSingleton : public ben::Singleton {
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
			EXPECT_EQ( &singleton1, &(index1_ptr->elem(singleton1.ID())) );
		
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
			EXPECT_EQ( singleton2_ptr, &(index1_ptr->elem(singleton2_ptr->ID())) );
		
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
			EXPECT_EQ( &singleton5, &(index1_ptr->elem(singleton5.ID())) );
		
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
			EXPECT_EQ( &singleton6, &(index1_ptr->elem(singleton6.ID())) );
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
			EXPECT_EQ( &singleton5, &(index1_ptr->elem(singleton5.ID())) );
		
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
			EXPECT_EQ(2, index2_ptr->size()); 
			//both
			EXPECT_TRUE( index1_ptr->manages(singleton8.ID()) ); 
			EXPECT_TRUE( index2_ptr->manages(singleton9.ID()) ); 
			EXPECT_TRUE( index1_ptr->check(singleton8.ID(), &singleton8) ); 
			EXPECT_TRUE( index2_ptr->check(singleton9.ID(), &singleton9) ); 
			EXPECT_EQ( &singleton8, &(index1_ptr->elem(singleton8.ID())) ); 
			EXPECT_EQ( &singleton9, &(index2_ptr->elem(singleton9.ID())) ); 
		
		//actual move
		singleton9 = std::move(singleton8);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_FALSE(singleton8.is_managed());
			//singleton5 tests
			EXPECT_EQ(1003, singleton9.ID());
			EXPECT_TRUE(singleton9.is_managed());
			EXPECT_EQ(index2_ptr, singleton9.get_index());
			//index2 tests
			EXPECT_EQ(1, index2_ptr->size());
			EXPECT_TRUE( index2_ptr->manages(singleton9.ID()) );
	}
	
	TEST(IndexSingleton, LocalMethods) {
	/*	using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef DerivedIndex<singleton_type> index_type;

			
		Index<DerivedSingleton> index1, index2;
		DerivedSingleton singleton1(index1), singleton2(index1, 5000), singleton3(index2, 5000);
		
		//test switch_index, valid
		//note: replaced singleton::switch_index with index::add; these tests may be redundant
		EXPECT_TRUE( index2.add(singleton1) );
			//index2 checks
			EXPECT_TRUE( index2.manages(singleton1.ID()) );
			EXPECT_TRUE( index2.check(singleton1.ID(), &singleton1) );
			//index1 checks
			EXPECT_FALSE( index1.manages(singleton1.ID()) );
			//singleton1 checks
			EXPECT_TRUE( singleton1.is_managed() );
			EXPECT_TRUE( singleton1.is_managed_by(index2) );
			
		//test switch_index, invalid
		EXPECT_FALSE( index2.add(singleton2) );
			//index2 checks
			EXPECT_FALSE( index2.check(singleton2.ID(), &singleton2) );
			//index1 checks
			EXPECT_TRUE( index1.manages(singleton2.ID()) );
			EXPECT_TRUE( index1.check(singleton2.ID(), &singleton2) );
			//singleton2 checks
			EXPECT_TRUE( singleton2.is_managed() );
			EXPECT_TRUE( singleton2.is_managed_by(index1) );
		
		//final checks
		EXPECT_EQ(1, index1.size());
		EXPECT_EQ(2, index2.size());
		EXPECT_TRUE( index2.manages(singleton3.ID()) );
		EXPECT_TRUE( index2.check(singleton3.ID(), &singleton3) );
		EXPECT_TRUE( singleton3.is_managed_by(index2) ); */
	}
	
	TEST(IndexSingleton, Iterators) {
		using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef DerivedIndex<singleton_type> index_type;

		auto index1_ptr = std::make_shared<index_type>();
		DerivedSingleton singleton1(index1_ptr), singleton2(index1_ptr), singleton3(index1_ptr);
		
		for(auto& x : *index1_ptr) EXPECT_TRUE( index1_ptr->manages(x.ID()) );

		index_type::iterator it = index1_ptr->begin();
		it++;
		EXPECT_TRUE( index1_ptr->manages(it->ID()) ); 
		++it;
		EXPECT_TRUE( index1_ptr->manages(it->ID()) ); 
	}
	
	TEST(IndexSingleton, GlobalMethods) {
		using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef DerivedIndex<singleton_type> index_type;

		auto index1_ptr = std::make_shared<index_type>();
		DerivedSingleton singleton1(index1_ptr), singleton2(index1_ptr), singleton3(index1_ptr, 5000);
		auto index2_ptr = std::make_shared<index_type>();
		DerivedSingleton singleton4(index2_ptr), singleton5(index2_ptr, 5003), singleton6(index2_ptr, 5000);
		
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
		index_type::iterator iter1 = index1_ptr->begin(); 
		EXPECT_TRUE(index1_ptr->find(iter1->ID()) == iter1); //gtest problem with expect_eq
		EXPECT_TRUE(index1_ptr->end() == index1_ptr->find(5001));
		//EXPECT_EQ(index1.find(5000), iter1);
		//EXPECT_EQ(index1.end(), index1.find(5001));
		
		//add test
		DerivedSingleton singleton7(5002);
		//null -> is_managed
		EXPECT_TRUE(singleton7.join_index(index1_ptr));
		EXPECT_TRUE(index1_ptr->check(5002, &singleton7));
		EXPECT_EQ(index1_ptr, singleton7.get_index());
		
		//is_managed -> is_managed redundant
		EXPECT_FALSE(singleton6.join_index(index1_ptr));
		EXPECT_FALSE(index1_ptr->check(5000, &singleton6));
		EXPECT_TRUE(index2_ptr->check(5000, &singleton6));
		EXPECT_EQ(index2_ptr, singleton6.get_index());
		
		//is_managed -> managed
		EXPECT_TRUE(singleton5.join_index(index1_ptr));
		EXPECT_TRUE(index1_ptr->check(5003, &singleton5));
		EXPECT_FALSE(index2_ptr->manages(5003));
		EXPECT_EQ(index1_ptr, singleton5.get_index());	
		
		//merge (redundant for id=5000)
		EXPECT_FALSE(merge(index2_ptr, index1_ptr));
			//index1 tests
			EXPECT_EQ(5, index1_ptr->size());
			EXPECT_TRUE(index1_ptr->check(singleton1.ID(), &singleton1));
			EXPECT_EQ(index1_ptr, singleton1.get_index());
			EXPECT_TRUE(index1_ptr->check(singleton2.ID(), &singleton2));
			EXPECT_EQ(index1_ptr, singleton2.get_index());
			EXPECT_TRUE(index1_ptr->check(5000, &singleton3));
			EXPECT_EQ(index1_ptr, singleton3.get_index());
			EXPECT_TRUE(index1_ptr->check(5003, &singleton5));
			EXPECT_EQ(index1_ptr, singleton5.get_index());
			EXPECT_TRUE(index1_ptr->check(5002, &singleton7));
			EXPECT_EQ(index1_ptr, singleton7.get_index());
			//index2 tests
			EXPECT_EQ(2, index2_ptr->size());
			EXPECT_TRUE(index2_ptr->check(singleton4.ID(), &singleton4));
			EXPECT_EQ(index2_ptr, singleton4.get_index());
			EXPECT_TRUE(index2_ptr->check(5000, &singleton6));
			EXPECT_EQ(index2_ptr, singleton6.get_index());
		
		//remove
		singleton3.leave_index(); 
		EXPECT_FALSE(singleton3.is_managed());
		EXPECT_FALSE(index1_ptr->manages(5000));
		
		//merge_into (valid)
		EXPECT_TRUE(merge(index2_ptr, index1_ptr)); 
			//index1 tests 
			EXPECT_EQ(0, index1_ptr->size());
			//index2 tests
			EXPECT_EQ(6, index2_ptr->size());
			EXPECT_TRUE(index2_ptr->check(singleton1.ID(), &singleton1));
			EXPECT_EQ(index2_ptr, singleton1.get_index());
			EXPECT_TRUE(index2_ptr->check(singleton2.ID(), &singleton2));
			EXPECT_EQ(index2_ptr, singleton2.get_index());
			EXPECT_TRUE(index2_ptr->check(singleton4.ID(), &singleton4));
			EXPECT_EQ(index2_ptr, singleton4.get_index());
			EXPECT_TRUE(index2_ptr->check(5003, &singleton5));
			EXPECT_EQ(index2_ptr, singleton5.get_index());
			EXPECT_TRUE(index2_ptr->check(5000, &singleton6));
			EXPECT_EQ(index2_ptr, singleton6.get_index());
			EXPECT_TRUE(index2_ptr->check(5002, &singleton7));
			EXPECT_EQ(index2_ptr, singleton7.get_index());
	}
	
} //anonymous namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

