/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

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
    
    e-mail: jackwhall7@gmail.com
*/

#include "Benoit.h"
#include <iostream>
#include <list>
#include "test_Node_Index_constructors.cpp"
#include "test_Link_mangement.cpp"
#include "test_data_transmission.cpp"
#include "test_Node_management.cpp"

//To test Benoit, run the following in the test directory:
//	g++ -std=c++0x -g -I ../src test_benoit.cpp -o test_benoit
//	./test

int main() {
	using namespace std;
	
	list<int> results;
	results.push_back( test_Node_Index_constructors() ); //need to update
	results.push_back( test_Link_mangement() ); //need to update
	results.push_back( test_data_transmission() ); //need to update
	results.push_back( test_Node_management() ); //need to update
	
	int test_number = 1;
	bool all_tests_passed = true;
	auto it = results.begin();
	auto ite = results.end();
	while(it != ite) {
		cout << "Test " << test_number;
		if(*it == 0) {
			cout << " passed." << endl;
		} else {
			cout << " failed with error code " << *it << "." << endl;
			all_tests_passed = false;
		}
		++it;
		++test_number;
	}
	
	if(all_tests_passed) cout << "All tests passed." << endl;
	return 0;
} //main

