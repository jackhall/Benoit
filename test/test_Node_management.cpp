#ifndef test_four_cpp
#define test_four_cpp

int test_Node_management() { 
	using namespace ben;
	
	Node<double,double> node_one, node_two, node_three;
	node_one.bias = 11.0;
	node_two.bias = 13.0;
	node_three.bias = 17.0;
	
	node_three.add_input(node_one.ID, 3.0);
	node_three.add_input(node_two.ID, 5.0);
	node_two.add_input(node_one.ID, 7.0);
	
	Index<double,double> index_one;
	
	//Node(Node&&)
	Node<double,double> node_four(std::move(node_two));
	//node_two is now useless because its const ID matches node_four
	if( node_four.input_begin()->origin != node_one.ID ) return 1;
	if( node_four.output_begin()->target != node_three.ID ) return 2;
	auto po = node_one.output_begin(); ++po;
	auto pi = node_four.input_begin();
	if( &*po != &*pi ) return 3;
	
	//Node::operator=(Node&)
	Node<double,double> node_five;
	node_five = node_four;
	if( node_five.input_begin()->origin != node_one.ID ) return 4;
	//only inputs copied (change this later)
	if( node_five.ID == node_four.ID ) return 5;
	
	//swap_with
	index_one.swap_with(Node<double,double>::INDEX);
	if( 	!index_one.contains(node_one.ID) || 
		!index_one.contains(node_three.ID) ||
		!index_one.contains(node_four.ID) ||
		!index_one.contains(node_five.ID) ) return 6;
	if( Node<double,double>::INDEX.contains(node_five.ID) ) return 7;
	//spot checks of Links
	po = node_one.output_begin();
	if( po->target != node_three.ID ) return 8; ++po; 
	if( po->target != node_four.ID ) return 9;  ++po;
	if( po->target != node_five.ID ) return 10; 
	
	//merge_into
	Node<double,double> node_six;
	index_one.merge_into(Node<double,double>::INDEX);
	node_six.add_input(node_four.ID, 19.0);
	node_six.add_input(node_five.ID, 23.0);
	if( !Node<double,double>::INDEX.contains(node_three.ID) ) return 11;
	if( !Node<double,double>::INDEX.contains(node_six.ID) ) return 12;
	if( index_one.contains(node_three.ID) ) return 13;
	if( node_five.input_begin()->origin != node_one.ID ) return 14;
	
	//Index(Index&&)
	Index<double,double> index_two(std::move(Node<double,double>::INDEX));
	if( !index_two.contains(node_three.ID) ) return 15;
	if( !index_two.contains(node_six.ID) ) return 16;
	if( Node<double,double>::INDEX.contains(node_three.ID) ) return 17;
	if( node_six.input_begin()->origin != node_four.ID ) return 18;
	
	//Index::operator=(Index&&)
	index_one = std::move(index_two);
	if( !index_one.contains(node_four.ID) ) return 19;
	if( index_two.contains(node_one.ID) ) return 20;
	
	//move_to
	index_one.move_to(index_two, node_four.ID);
	if( !index_two.contains(node_four.ID) ) return 21;
	if( !index_one.contains(node_five.ID) ) return 22;
	if( node_four.input_begin() != node_four.input_end() ) return 23;
	if( node_four.output_begin() != node_four.output_end() ) return 24;
	if( node_six.input_begin()->origin != node_five.ID ) return 25;
	po = node_one.output_begin(); ++po;
	if( po->target == node_four.ID ) return 26;
	
	return 0;
} //test_four

#endif
