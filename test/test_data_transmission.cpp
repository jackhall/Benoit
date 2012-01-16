#ifndef test_three_cpp
#define test_three_cpp

int test_data_transmission() { 
	using namespace ben;
	
	Node<double,double> node_one, node_two, node_three;
	node_one.bias = 11.0;
	node_two.bias = 13.0;
	node_three.bias = 17.0;
	
	node_three.add_input(node_one.ID,3.0);
	node_three.add_input(node_two.ID,5.0);
	node_two.add_input(node_one.ID,7.0);
	
	//send signal from node_one
	auto po1 = node_one.output_begin();
	auto po1e = node_one.output_end();
	while(po1 != po1e) {
		po1 << 19.0 << 23.0; //23.0 is time-delayed
		++po1;
	}
	
	//receive data at node_two
	auto pi2 = node_two.input_begin();
	auto pi2e = node_two.input_end();
	double x, z=node_two.bias;
	while(pi2 != pi2e) {
		pi2 >> x;
		z *= pi2->weight * x;
		++pi2;
	} 
	if( z != 13.0*(7.0*19.0) ) return 1;
	
	//send product of all numbers to node_three
	auto po2 = node_two.output_begin();
	auto po2e = node_two.output_end();
	while(po2 != po2e) {
		po2 << z;
		++po2;
	}
	
	//receive signals from node_one and node_two at node_three
	auto pi3 = node_three.input_begin();
	auto pi3e = node_three.input_end();
	z = node_three.bias;
	while(pi3 != pi3e) {
		pi3 >> x;
		z *= pi3->weight * x;
		++pi3;
	}
	if( z != 17.0*(3.0*19.0)*(5.0*13.0*(7.0*19.0)) ) return 2;
	
	//transmit from node_one again
	po1 = node_one.output_begin();
	while(po1 != po1e) {
		po1 << 29.0;
		++po1;
	}
	
	//test time delay too
	pi2 = node_two.input_begin();
	z = node_two.bias;
	while(pi2 != pi2e) {
		pi2 >> x;
		z *= pi2->weight * x;
		++pi2;
	}
	if( z != 13.0*(7.0*23.0) ) return 3;
	
	return 0;
} //test_three

#endif
