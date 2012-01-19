#include "Benoit.h"
#include <iostream>
#include <list>
#include "test_Node_Index_constructors.cpp"
#include "test_Link_mangement.cpp"
#include "test_data_transmission.cpp"
#include "test_Node_management.cpp"
//(c) Jack Hall 2011, licensed under GNU GPL v3

//To test Benoit, run the following in the test directory:
//	g++ -std=c++0x -g -I ../src test_benoit.cpp -o test
//	./test

int main() {
	using namespace std;
	
	list<int> results;
	results.push_back( test_Node_Index_constructors() ); //need to update
	results.push_back( test_Link_mangement() ); //need to update
	results.push_back( test_data_transmission() );
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

