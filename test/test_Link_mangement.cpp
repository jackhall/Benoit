#ifndef test_two_cpp
#define test_two_cpp

int test_Link_mangement() { 
	using namespace ben;
	
	Node<double,double> node_one;
	node_one.bias = 3.0;
	Node<double,double>* pnode_two = new Node<double,double>();
	pnode_two->bias = 5.0;
	
	//test basic add/remove
	node_one.add_input(pnode_two->ID,7.0);
	if( !node_one.contains_input(pnode_two->ID) ) return 1;
	if( !pnode_two->contains_output(node_one.ID) ) return 2;
	
	node_one.remove_input(pnode_two->ID); 
	if( node_one.contains_input(pnode_two->ID) ) return 3;
	if( pnode_two->contains_output(node_one.ID) ) return 4;
	
	//test self-Linking
	pnode_two->add_input(pnode_two->ID,11.0);
	if( !pnode_two->contains_input(pnode_two->ID) ) return 5;
	if( !pnode_two->contains_output(pnode_two->ID) ) return 6;
	
	//test basic clear on self-Link
	pnode_two->clear();
	
	//test clearing from output end
	node_one.add_input(pnode_two->ID,13.0);
	pnode_two->clear();
	if( node_one.contains_input(pnode_two->ID) ) return 7;
	
	//test clearing from input end
	node_one.add_input(pnode_two->ID,17.0);
	node_one.clear(); 
	if( pnode_two->contains_output(node_one.ID) ) return 8;
	
	delete pnode_two;
	
	return 0;
} //test_two

#endif
