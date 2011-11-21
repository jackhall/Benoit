#include "Benoit.h"

//g++ -std=c++0x -I ~/Code/Benoit/headers -I ~/Code/Benoit/source test_benoit.cpp -o test


int main() {
	using namespace ben;
	Node<double, double, double>* one = new Node<double, double, double>();
	
	delete one;
	return 0;
}
