#ifndef test_one_cpp
#define test_one_cpp

int test_Node_Index_constructors() { 
	
	using namespace ben;
	
	//testing Node default constructor on stack
	Node<int,int> dummy_node;
	if( !Node<int,int>::INDEX.contains(dummy_node.ID) ) return 1;
	
	//testing Index default constructor
	Index<double,double> first_index;
	
	//testing Node default constructor with new
	Node<double,double>* pfirst_node = new Node<double,double>();
	if( !Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 2;
	
	//testing Index::move_to
	Node<double,double>::INDEX.move_to(first_index, pfirst_node->ID);
	if( Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 3;
	if( !first_index.contains(pfirst_node->ID) ) return 4;
	
	//testing Node(Index*) constructor
	Node<double,double>* psecond_node = new Node<double,double>(first_index);
	if( !first_index.contains(psecond_node->ID) ) return 5;
	
	//testing Node move constructor
	unsigned int second_ID = psecond_node->ID;
	Node<double,double>* pthird_node = new Node<double,double>(std::move(*psecond_node));
	if( pthird_node->ID != second_ID ) return 6;
	if( first_index.find(psecond_node->ID) == psecond_node ) return 7;
	if( first_index.find(pthird_node->ID) != pthird_node ) return 8;
	delete psecond_node; //psecond_node shares an ID with pthird_node, so delete it
	psecond_node = NULL; 
	
	//testing Node::operator= (creates new copies of links) 
	Node<double,double>* pfourth_node = new Node<double,double>();
	*pthird_node = *pfourth_node; 
	if( first_index.contains(pthird_node->ID) ) return 9;
	if( !Node<double,double>::INDEX.contains(pthird_node->ID) ) return 10;
	
	//testing Index copy constructor
	Index<double,double>* psecond_index = new Index<double,double>(std::move(first_index));
	if( !psecond_index->contains(pfirst_node->ID) ) return 11;
	
	//testing destructors
	delete psecond_index;
	if( !Node<double,double>::INDEX.contains(pfirst_node->ID) ) return 12;
	delete pfirst_node;
	delete pthird_node;
	delete pfourth_node;
	
	return 0;
} //test_one

#endif
