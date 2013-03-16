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

	struct DerivedSingleton : public ben::Singleton {
		typedef ben::Index<DerivedSingleton> index_type;
		typedef ben::Singleton base_type;

		DerivedSingleton() : Singleton() {}
		DerivedSingleton(const id_type id) : Singleton(id) {}
		DerivedSingleton(std::shared_ptr<index_type> x) : Singleton(x) {}
		DerivedSingleton(std::shared_ptr<index_type> x, const id_type id) : Singleton(x, id) {}
		DerivedSingleton(const DerivedSingleton& rhs) = delete;
		DerivedSingleton(DerivedSingleton&& rhs) : Singleton( std::move(rhs) ) {}
		DerivedSingleton& operator=(const DerivedSingleton& rhs) = delete;
		DerivedSingleton& operator=(DerivedSingleton&& rhs) { Singleton::operator=( std::move(rhs) ); 
		~DerivedSingleton() = default;
		
		void join_index(std::shared_ptr<index_type> x) { base_type::join_index(x); }
		std::shared_ptr<index_type> get_index() const { return static_pointer_cast(base_type::get_index()); }	
	};

	TEST(IndexSingleton, Construction) {
		using namespace ben;
		typedef DerivedSingleton singleton_type;
		typedef Index<singleton_type> index_type;
		
		//======= Index<> default constructor =========
		auto index1_ptr = make_shared<index_type>();
			EXPECT_EQ(0, index1_ptr->size());
		
		//======= Singleton index constructor ========
		singleton_type singleton1(index1_ptr);
			//singleton1 tests
			EXPECT_EQ(1000, singleton1.ID());
			EXPECT_TRUE(singleton1.is_managed());
			EXPECT_TRUE(index1_ptr, singleton1.get_index());
			//index1 tests
			EXPECT_EQ(1, index1_ptr->size());
			//both
			EXPECT_TRUE( index1_ptr->contains(singleton1.ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton1.ID(), &singleton1) );
			EXPECT_EQ( &singleton1, &(index1_ptr->elem(singleton1.ID())) );
		
		//====== Index<> destructor ========= //unnecessary; managed Nodes now have shared ownership
		//
		//====== Singleton index constructor ====== 
		singleton_type* singleton2_ptr = new singleton_type(index1_ptr);
			//singleton2 tests
			EXPECT_EQ(1001, singleton2_ptr->ID());
			EXPECT_TRUE(singleton2_ptr->is_managed());
			EXPECT_TRUE(index1_ptr, singleton2_ptr->get_index());
			//index2 tests
			EXPECT_EQ(1, index1_ptr->size());
			//both
			EXPECT_TRUE( index2.contains(singleton2_ptr->ID()) );
			EXPECT_TRUE( index2.check(singleton2_ptr->ID(), singleton2_ptr) );
			EXPECT_EQ( singleton2_ptr, &(index2.elem(singleton2_ptr->ID())) );
		
		//====== Singleton destructor =======
		unsigned short id2 = singleton2_ptr->ID();
		delete singleton2_ptr;
		singleton2_ptr = nullptr;
			//index2 tests
			EXPECT_EQ(0, index2.size());
			EXPECT_FALSE(index2.contains(id2));
		
		//====== Singleton default constructor ======
		singleton_type singleton3;
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.is_managed());
		
		//====== Singleton id constructor ======
		singleton_type singleton4(5000);
			EXPECT_EQ(5000, singleton4.ID());
			EXPECT_FALSE(singleton4.is_managed());
		
		//====== Singleton index+id constructor ======
		singleton_type singleton5(index2, 6000);
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.is_managed());
			EXPECT_TRUE(singleton5.is_managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			//both
			EXPECT_TRUE( index2.contains(singleton5.ID()) );
			EXPECT_TRUE( index2.check(singleton5.ID(), &singleton5) );
			EXPECT_EQ( &singleton5, &(index2.elem(singleton5.ID())) );
		
		//====== Singleton move constructor ======
		//---- from is_managed
		singleton_type singleton6( std::move(singleton5) );
			//singleton6 tests
			EXPECT_EQ(6000, singleton6.ID());
			EXPECT_TRUE(singleton6.is_managed());
			EXPECT_TRUE(singleton6.is_managed_by(index2));
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_FALSE(singleton5.is_managed());
			EXPECT_FALSE(singleton5.is_managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			EXPECT_TRUE( index2.contains(singleton6.ID()) );
			//singleton6+index2 tests
			EXPECT_TRUE( index2.check(singleton6.ID(), &singleton6) );
			EXPECT_EQ( &singleton6, &(index2.elem(singleton6.ID())) );
			//singleton5+index2 tests
			EXPECT_FALSE( index2.check(singleton5.ID(), &singleton5) );
		
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
			EXPECT_FALSE(singleton6.is_managed_by(index2));
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.is_managed());
			EXPECT_TRUE(singleton5.is_managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			EXPECT_TRUE( index2.contains(singleton6.ID()) );
			//singleton6+index2 tests
			EXPECT_FALSE( index2.check(singleton6.ID(), &singleton6) );
			//singleton5+index2 tests
			EXPECT_TRUE( index2.check(singleton5.ID(), &singleton5) );
			EXPECT_EQ( &singleton5, &(index2.elem(singleton5.ID())) );
		
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
			//index2 tests
			EXPECT_TRUE(index2.empty());
			EXPECT_EQ(0, index2.size());
			EXPECT_FALSE( index2.contains(singleton5.ID()) );
		
		//---- is_managed -> is_managed
		//prepare new singleton
		singleton_type singleton8(index2); 
		singleton_type singleton9(index2);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_TRUE(singleton8.is_managed());
			EXPECT_TRUE(singleton8.is_managed_by(index2));
			//singleton9 tests
			EXPECT_EQ(1004, singleton9.ID());
			EXPECT_TRUE(singleton9.is_managed());
			EXPECT_TRUE(singleton9.is_managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(2, index2.size()); 
			//both
			EXPECT_TRUE( index2.contains(singleton8.ID()) ); 
			EXPECT_TRUE( index2.contains(singleton9.ID()) ); 
			EXPECT_TRUE( index2.check(singleton8.ID(), &singleton8) ); 
			EXPECT_TRUE( index2.check(singleton9.ID(), &singleton9) ); 
			EXPECT_EQ( &singleton8, &(index2.elem(singleton8.ID())) ); 
			EXPECT_EQ( &singleton9, &(index2.elem(singleton9.ID())) ); 
		
		//actual move
		singleton9 = std::move(singleton8);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_FALSE(singleton8.is_managed());
			//singleton5 tests
			EXPECT_EQ(1003, singleton9.ID());
			EXPECT_TRUE(singleton9.is_managed());
			EXPECT_TRUE(singleton9.is_managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			EXPECT_TRUE( index2.contains(singleton9.ID()) );
	}
	
	TEST(IndexSingleton, LocalMethods) {
		using namespace ben;
	
		Index<DerivedSingleton> index1, index2;
		DerivedSingleton singleton1(index1), singleton2(index1, 5000), singleton3(index2, 5000);
		
		//test switch_index, valid
		//note: replaced singleton::switch_index with index::add; these tests may be redundant
		EXPECT_TRUE( index2.add(singleton1) );
			//index2 checks
			EXPECT_TRUE( index2.contains(singleton1.ID()) );
			EXPECT_TRUE( index2.check(singleton1.ID(), &singleton1) );
			//index1 checks
			EXPECT_FALSE( index1.contains(singleton1.ID()) );
			//singleton1 checks
			EXPECT_TRUE( singleton1.is_managed() );
			EXPECT_TRUE( singleton1.is_managed_by(index2) );
			
		//test switch_index, invalid
		EXPECT_FALSE( index2.add(singleton2) );
			//index2 checks
			EXPECT_FALSE( index2.check(singleton2.ID(), &singleton2) );
			//index1 checks
			EXPECT_TRUE( index1.contains(singleton2.ID()) );
			EXPECT_TRUE( index1.check(singleton2.ID(), &singleton2) );
			//singleton2 checks
			EXPECT_TRUE( singleton2.is_managed() );
			EXPECT_TRUE( singleton2.is_managed_by(index1) );
		
		//final checks
		EXPECT_EQ(1, index1.size());
		EXPECT_EQ(2, index2.size());
		EXPECT_TRUE( index2.contains(singleton3.ID()) );
		EXPECT_TRUE( index2.check(singleton3.ID(), &singleton3) );
		EXPECT_TRUE( singleton3.is_managed_by(index2) );
	}
	
	TEST(IndexSingleton, Iterators) {
		using namespace ben;
		
		Index<DerivedSingleton> index1;
		DerivedSingleton singleton1(index1), singleton2(index1), singleton3(index1);
		
		for(DerivedSingleton& x : index1) EXPECT_TRUE( index1.contains(x.ID()) );

		const Index<DerivedSingleton>& index2 = index1;
		
		for(const DerivedSingleton& x : index2) EXPECT_TRUE( index2.contains(x.ID()) );
		
		//--it; --cit;
		//it--; cit--;
		auto it = index1.begin();
		auto cit = index2.begin();
		it++; cit++;
		
		EXPECT_TRUE(it == cit);
		EXPECT_TRUE(cit == it);
		EXPECT_FALSE(it != cit);
		EXPECT_FALSE(cit != it);
		
		//test out std::for_each
	}
	
	TEST(IndexSingleton, GlobalMethods) {
		using namespace ben;
		
		Index<DerivedSingleton> index1, index2;
		DerivedSingleton singleton1(index1), singleton2(index1), singleton3(index1, 5000);
		DerivedSingleton singleton4(index2), singleton5(index2, 5003), singleton6(index2, 5000);
		
		const Index<DerivedSingleton>& index3 = index1;
		
		//initial checks
		EXPECT_EQ(3, index1.size());
		EXPECT_EQ(3, index2.size());
		EXPECT_TRUE(singleton1.is_managed_by(index1));
		EXPECT_TRUE(singleton2.is_managed_by(index1));
		EXPECT_TRUE(singleton3.is_managed_by(index1));
		EXPECT_TRUE(singleton4.is_managed_by(index2));
		EXPECT_TRUE(singleton5.is_managed_by(index2));
		EXPECT_TRUE(singleton6.is_managed_by(index2));
		
		//find tests
		auto iter1 = index1.begin(); 
		EXPECT_TRUE(index1.find(iter1->ID()) == iter1); //gtest problem with expect_eq
		EXPECT_TRUE(index1.end() == index1.find(5001));
		//EXPECT_EQ(index1.find(5000), iter1);
		//EXPECT_EQ(index1.end(), index1.find(5001));
		
		//find const tests
		auto iter2 = index3.begin();
		EXPECT_TRUE(index3.find(iter2->ID()) == iter2); //gtest problem with expect_eq
		EXPECT_TRUE(index3.end() == index3.find(5001));
		//EXPECT_EQ(index3.find(5000), iter2);
		//EXPECT_EQ(index3.end(), index3.find(5001));
		
		//add test
		DerivedSingleton singleton7(5002);
		//null -> is_managed
		EXPECT_TRUE(index1.add(singleton7));
		EXPECT_TRUE(index1.check(5002, &singleton7));
		EXPECT_TRUE(singleton7.is_managed_by(index1));
		
		//is_managed -> is_managed redundant
		EXPECT_FALSE(index1.add(singleton6));
		EXPECT_FALSE(index1.check(5000, &singleton6));
		EXPECT_TRUE(index2.check(5000, &singleton6));
		EXPECT_TRUE(singleton6.is_managed_by(index2));
		
		//is_managed -> mangaged
		EXPECT_TRUE(index1.add(singleton5));
		EXPECT_TRUE(index1.check(5003, &singleton5));
		EXPECT_FALSE(index2.contains(5003));
		EXPECT_TRUE(singleton5.is_managed_by(index1));	
		
		//merge_into (redundant for id=5000)
		EXPECT_FALSE(index1.merge_into(index2));
			//index1 tests
			EXPECT_EQ(5, index1.size());
			EXPECT_TRUE(index1.check(singleton1.ID(), &singleton1));
			EXPECT_TRUE(singleton1.is_managed_by(index1));
			EXPECT_TRUE(index1.check(singleton2.ID(), &singleton2));
			EXPECT_TRUE(singleton2.is_managed_by(index1));
			EXPECT_TRUE(index1.check(5000, &singleton3));
			EXPECT_TRUE(singleton3.is_managed_by(index1));
			EXPECT_TRUE(index1.check(5003, &singleton5));
			EXPECT_TRUE(singleton5.is_managed_by(index1));
			EXPECT_TRUE(index1.check(5002, &singleton7));
			EXPECT_TRUE(singleton7.is_managed_by(index1));
			//index2 tests
			EXPECT_EQ(2, index2.size());
			EXPECT_TRUE(index2.check(singleton4.ID(), &singleton4));
			EXPECT_TRUE(singleton4.is_managed_by(index2));
			EXPECT_TRUE(index2.check(5000, &singleton6));
			EXPECT_TRUE(singleton6.is_managed_by(index2));
		
		//remove
		EXPECT_TRUE(index1.remove(5000)); 
		EXPECT_FALSE(singleton3.is_managed());
		EXPECT_FALSE(index1.contains(5000));
		EXPECT_FALSE(index1.remove(6000));
		
		//merge_into (valid)
		EXPECT_TRUE(index1.merge_into(index2)); //segfaults
			//index1 tests 
			EXPECT_EQ(0, index1.size());
			//index2 tests
			EXPECT_EQ(6, index2.size());
			EXPECT_TRUE(index2.check(singleton1.ID(), &singleton1));
			EXPECT_TRUE(singleton1.is_managed_by(index2));
			EXPECT_TRUE(index2.check(singleton2.ID(), &singleton2));
			EXPECT_TRUE(singleton2.is_managed_by(index2));
			EXPECT_TRUE(index2.check(singleton4.ID(), &singleton4));
			EXPECT_TRUE(singleton4.is_managed_by(index2));
			EXPECT_TRUE(index2.check(5003, &singleton5));
			EXPECT_TRUE(singleton5.is_managed_by(index2));
			EXPECT_TRUE(index2.check(5000, &singleton6));
			EXPECT_TRUE(singleton6.is_managed_by(index2));
			EXPECT_TRUE(index2.check(5002, &singleton7));
			EXPECT_TRUE(singleton7.is_managed_by(index2));
			
		//clear
		index2.clear();
		EXPECT_EQ(0, index2.size());
		EXPECT_FALSE(singleton1.is_managed());
		EXPECT_FALSE(singleton2.is_managed());
		EXPECT_FALSE(singleton4.is_managed());
		EXPECT_FALSE(singleton5.is_managed());
		EXPECT_FALSE(singleton6.is_managed());
		EXPECT_FALSE(singleton7.is_managed()); 
	}
	
} //anonymous namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

