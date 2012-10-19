/*
	Benoit: distributed graph and space data structures
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
		DerivedSingleton(index_type& x) : Singleton(x) {}
		DerivedSingleton(index_type& x, const id_type id) : Singleton(x, id) {}
		DerivedSingleton(const DerivedSingleton& rhs) = delete;
		DerivedSingleton(DerivedSingleton&& rhs) : Singleton( std::move(rhs) ) {}
		DerivedSingleton& operator=(const DerivedSingleton& rhs) = delete;
		DerivedSingleton& operator=(DerivedSingleton&& rhs) { Singleton::operator=( std::move(rhs) ); }
		~DerivedSingleton() = default;
		
		bool switch_index(index_type* ptr) { return base_type::switch_index(ptr); }
		using base_type::managed;
		bool managed_by(const index_type& x) { return base_type::managed_by(x); }
		using base_type::ID;
	};

	TEST(IndexSingleton, Construction) {
		using namespace ben;
		
		//======= Index<> default constructor =========
		Index<DerivedSingleton>* index1_ptr = new Index<DerivedSingleton>();
			EXPECT_TRUE(index1_ptr->empty()); 
			EXPECT_EQ(0, index1_ptr->size());
		
		//======= Singleton index constructor ========
		DerivedSingleton singleton1(*index1_ptr);
			//singleton1 tests
			EXPECT_EQ(1000, singleton1.ID());
			EXPECT_TRUE(singleton1.managed());
			EXPECT_TRUE(singleton1.managed_by(*index1_ptr));
			//index1 tests
			EXPECT_FALSE(index1_ptr->empty()); 
			EXPECT_EQ(1, index1_ptr->size());
			//both
			EXPECT_TRUE( index1_ptr->contains(singleton1.ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton1.ID(), &singleton1) );
			EXPECT_EQ( &singleton1, &(index1_ptr->elem(singleton1.ID())) );
		
		//======= Index<> move constructor =========
		Index<DerivedSingleton> index2( std::move(*index1_ptr) );
			//singleton1 tests
			EXPECT_TRUE(singleton1.managed());
			EXPECT_TRUE(singleton1.managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			//index1 tests
			EXPECT_TRUE(index1_ptr->empty()); 
			EXPECT_EQ(0, index1_ptr->size());
			//singleton+index2
			EXPECT_TRUE( index2.contains(singleton1.ID()) );
			EXPECT_TRUE( index2.check(singleton1.ID(), &singleton1) );
			EXPECT_EQ( &singleton1, &(index2.elem(singleton1.ID())) );
			//singleton+index1
			EXPECT_FALSE( index1_ptr->contains(singleton1.ID()) );
			EXPECT_FALSE( index1_ptr->check(singleton1.ID(), &singleton1) );
			EXPECT_NE( &singleton1, &(index1_ptr->elem(singleton1.ID())) );
		
		//====== Index<> move assignment operator =======
		*index1_ptr = std::move(index2);
			//singleton1 tests
			EXPECT_TRUE(singleton1.managed());
			EXPECT_TRUE(singleton1.managed_by(*index1_ptr));
			//index1 tests
			EXPECT_FALSE(index1_ptr->empty()); 
			EXPECT_EQ(1, index1_ptr->size());
			//index2 tests
			EXPECT_TRUE(index2.empty()); 
			EXPECT_EQ(0, index2.size());
			//singleton+index1
			EXPECT_TRUE( index1_ptr->contains(singleton1.ID()) );
			EXPECT_TRUE( index1_ptr->check(singleton1.ID(), &singleton1) );
			EXPECT_EQ( &singleton1, &(index1_ptr->elem(singleton1.ID())) );
			//singleton+index2
			EXPECT_FALSE( index2.contains(singleton1.ID()) );
			EXPECT_FALSE( index2.check(singleton1.ID(), &singleton1) );
			EXPECT_NE( &singleton1, &(index2.elem(singleton1.ID())) );
		
		//====== Index<> destructor =========
		delete index1_ptr;
		index1_ptr = nullptr;
			//singleton1 tests
			EXPECT_FALSE(singleton1.managed());
		
		//====== Singleton index constructor ====== 
		DerivedSingleton* singleton2_ptr = new DerivedSingleton(index2);
			//singleton2 tests
			EXPECT_EQ(1001, singleton2_ptr->ID());
			EXPECT_TRUE(singleton2_ptr->managed());
			EXPECT_TRUE(singleton2_ptr->managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			//both
			EXPECT_TRUE( index2.contains(singleton2_ptr->ID()) );
			EXPECT_TRUE( index2.check(singleton2_ptr->ID(), singleton2_ptr) );
			EXPECT_EQ( singleton2_ptr, &(index2.elem(singleton2_ptr->ID())) );
		
		//====== Singleton destructor =======
		unsigned short id2 = singleton2_ptr->ID();
		delete singleton2_ptr;
		singleton2_ptr = nullptr;
			//index2 tests
			EXPECT_TRUE(index2.empty());
			EXPECT_EQ(0, index2.size());
			EXPECT_FALSE(index2.contains(id2));
		
		//====== Singleton default constructor ======
		DerivedSingleton singleton3;
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.managed());
		
		//====== Singleton id constructor ======
		DerivedSingleton singleton4(5000);
			EXPECT_EQ(5000, singleton4.ID());
			EXPECT_FALSE(singleton4.managed());
		
		//====== Singleton index+id constructor ======
		DerivedSingleton singleton5(index2, 6000);
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.managed());
			EXPECT_TRUE(singleton5.managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			//both
			EXPECT_TRUE( index2.contains(singleton5.ID()) );
			EXPECT_TRUE( index2.check(singleton5.ID(), &singleton5) );
			EXPECT_EQ( &singleton5, &(index2.elem(singleton5.ID())) );
		
		//====== Singleton move constructor ======
		//---- from managed
		DerivedSingleton singleton6( std::move(singleton5) );
			//singleton6 tests
			EXPECT_EQ(6000, singleton6.ID());
			EXPECT_TRUE(singleton6.managed());
			EXPECT_TRUE(singleton6.managed_by(index2));
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_FALSE(singleton5.managed());
			EXPECT_FALSE(singleton5.managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			EXPECT_TRUE( index2.contains(singleton6.ID()) );
			//singleton6+index2 tests
			EXPECT_TRUE( index2.check(singleton6.ID(), &singleton6) );
			EXPECT_EQ( &singleton6, &(index2.elem(singleton6.ID())) );
			//singleton5+index2 tests
			EXPECT_FALSE( index2.check(singleton5.ID(), &singleton5) );
			EXPECT_NE( &singleton5, &(index2.elem(singleton5.ID())) );
		
		//---- from null
		DerivedSingleton singleton7( std::move(singleton3) ); 
			//singleton7 tests
			EXPECT_EQ(1002, singleton7.ID()); 
			EXPECT_FALSE(singleton7.managed());
			//singleton3 tests
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.managed());
		
		//====== Singleton move assignment operator ======
		//---- managed -> null
		singleton5 = std::move(singleton6);
			//singleton6 tests
			EXPECT_EQ(6000, singleton6.ID());
			EXPECT_FALSE(singleton6.managed());
			EXPECT_FALSE(singleton6.managed_by(index2));
			//singleton5 tests
			EXPECT_EQ(6000, singleton5.ID());
			EXPECT_TRUE(singleton5.managed());
			EXPECT_TRUE(singleton5.managed_by(index2));
			//index2 tests
			EXPECT_FALSE(index2.empty()); 
			EXPECT_EQ(1, index2.size());
			EXPECT_TRUE( index2.contains(singleton6.ID()) );
			//singleton6+index2 tests
			EXPECT_FALSE( index2.check(singleton6.ID(), &singleton6) );
			EXPECT_NE( &singleton6, &(index2.elem(singleton6.ID())) );
			//singleton5+index2 tests
			EXPECT_TRUE( index2.check(singleton5.ID(), &singleton5) );
			EXPECT_EQ( &singleton5, &(index2.elem(singleton5.ID())) );
		
		//---- null -> null
		singleton3 = std::move(singleton7);
			//singleton7 tests
			EXPECT_EQ(1002, singleton7.ID());
			EXPECT_FALSE(singleton7.managed());
			//singleton3 tests
			EXPECT_EQ(1002, singleton3.ID());
			EXPECT_FALSE(singleton3.managed());
		
		//---- null -> managed
		singleton5 = std::move(singleton4);
			//singleton3 tests
			EXPECT_EQ(5000, singleton5.ID());
			EXPECT_FALSE(singleton5.managed());
			//singleton4 tests
			EXPECT_EQ(5000, singleton4.ID());
			EXPECT_FALSE(singleton4.managed());
			//index2 tests
			EXPECT_TRUE(index2.empty());
			EXPECT_EQ(0, index2.size());
			EXPECT_FALSE( index2.contains(singleton5.ID()) );
		
		//---- managed -> managed
		//prepare new singleton
		DerivedSingleton singleton8(index2); 
		DerivedSingleton singleton9(index2);
			//singleton8 tests
			EXPECT_EQ(1003, singleton8.ID());
			EXPECT_TRUE(singleton8.managed());
			EXPECT_TRUE(singleton8.managed_by(index2));
			//singleton9 tests
			EXPECT_EQ(1004, singleton9.ID());
			EXPECT_TRUE(singleton9.managed());
			EXPECT_TRUE(singleton9.managed_by(index2));
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
			EXPECT_FALSE(singleton8.managed());
			//singleton5 tests
			EXPECT_EQ(1003, singleton9.ID());
			EXPECT_TRUE(singleton9.managed());
			EXPECT_TRUE(singleton9.managed_by(index2));
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
		EXPECT_TRUE( singleton1.switch_index(&index2) );
			//index2 checks
			EXPECT_TRUE( index2.contains(singleton1.ID()) );
			EXPECT_TRUE( index2.check(singleton1.ID(), &singleton1) );
			//index1 checks
			EXPECT_FALSE( index1.contains(singleton1.ID()) );
			//singleton1 checks
			EXPECT_TRUE( singleton1.managed() );
			EXPECT_TRUE( singleton1.managed_by(index2) );
			
		//test switch_index, invalid
		EXPECT_FALSE( singleton2.switch_index(&index2) );
			//index2 checks
			EXPECT_FALSE( index2.check(singleton2.ID(), &singleton2) );
			//index1 checks
			EXPECT_TRUE( index1.contains(singleton2.ID()) );
			EXPECT_TRUE( index1.check(singleton2.ID(), &singleton2) );
			//singleton2 checks
			EXPECT_TRUE( singleton2.managed() );
			EXPECT_TRUE( singleton2.managed_by(index1) );
		
		//final checks
		EXPECT_EQ(1, index1.size());
		EXPECT_EQ(2, index2.size());
		EXPECT_TRUE( index2.contains(singleton3.ID()) );
		EXPECT_TRUE( index2.check(singleton3.ID(), &singleton3) );
		EXPECT_TRUE( singleton3.managed_by(index2) );
	}
	
	TEST(IndexSingleton, Iterators) {
		using namespace ben;
		
		Index<DerivedSingleton> index1;
		DerivedSingleton singleton1(index1), singleton2(index1), singleton3(index1);
		
		auto it = index1.begin();
		auto ite = index1.end();
		unsigned short id = singleton1.ID();
		while(it != ite) {
			EXPECT_EQ(id, it->ID());
			++it; ++id;
		}
		
		const Index<DerivedSingleton>& index2 = index1;
		
		auto cit = index2.begin();
		auto cite = index2.end();
		id = singleton1.ID();
		while(cit != cite) {
			EXPECT_EQ(id, cit->ID());
			++cit; ++id;
		}
		
		--it; --cit;
		it--; cit--;
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
		EXPECT_TRUE(singleton1.managed_by(index1));
		EXPECT_TRUE(singleton2.managed_by(index1));
		EXPECT_TRUE(singleton3.managed_by(index1));
		EXPECT_TRUE(singleton4.managed_by(index2));
		EXPECT_TRUE(singleton5.managed_by(index2));
		EXPECT_TRUE(singleton6.managed_by(index2));
		
		//find tests
		auto iter1 = index1.end(); 
		--iter1;
		EXPECT_TRUE(index1.find(5000) == iter1); //gtest problem with expect_eq
		EXPECT_TRUE(index1.end() == index1.find(5001));
		//EXPECT_EQ(index1.find(5000), iter1);
		//EXPECT_EQ(index1.end(), index1.find(5001));
		
		//find const tests
		auto iter2 = index3.end();
		--iter2;
		EXPECT_TRUE(index3.find(5000) == iter2); //gtest problem with expect_eq
		EXPECT_TRUE(index3.end() == index3.find(5001));
		//EXPECT_EQ(index3.find(5000), iter2);
		//EXPECT_EQ(index3.end(), index3.find(5001));
		
		//add test
		DerivedSingleton singleton7(5002);
		//null -> managed
		EXPECT_TRUE(index1.add(singleton7));
		EXPECT_TRUE(index1.check(5002, &singleton7));
		EXPECT_TRUE(singleton7.managed_by(index1));
		
		//managed -> managed redundant
		EXPECT_FALSE(index1.add(singleton6));
		EXPECT_FALSE(index1.check(5000, &singleton6));
		EXPECT_TRUE(index2.check(5000, &singleton6));
		EXPECT_TRUE(singleton6.managed_by(index2));
		
		//managed -> mangaged
		EXPECT_TRUE(index1.add(singleton5));
		EXPECT_TRUE(index1.check(5003, &singleton5));
		EXPECT_FALSE(index2.contains(5003));
		EXPECT_TRUE(singleton5.managed_by(index1));	
		
		//merge_into
		EXPECT_FALSE(index1.merge_into(index2));
		//....
	}
	
} //anonymous namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

