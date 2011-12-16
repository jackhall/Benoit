template<typename W, typename S, typename E>
Node<W,S,E>::Node() 
	: ID( getNewID() ), index( &(Node::INDEX) ) {
	index->add(ID,this);
}

template<typename W, typename S, typename E>
Node<W,S,E>::Node(const Index<W,S,E>* pIndex) 
	: ID( getNewID() ), index(pIndex) {
	//add self to manager
	index->add(ID,this);
}

template<typename W, typename S, typename E>
Node<W,S,E>::~Node() {
	//delete all Links (input and output)
	clear();
	//remove self from manager
	index->remove(ID);
}
