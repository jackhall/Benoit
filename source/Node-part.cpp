#ifndef BenoitNode_cpp
#define BenoitNode_cpp
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	//=============== CTOR, DTOR ===================
	template<typename W, typename S, typename E>
	Node<W,S,E>::Node() 
		: ID( getNewID() ), index( &(Node::INDEX) ) {
		index->add(this);
	} //default constructor

	template<typename W, typename S, typename E>
	Node<W,S,E>::Node(Index<W,S,E>* const pIndex) 
		: ID( getNewID() ), index(pIndex) {
		//add self to manager
		index->add(this);
	} //constructor

	template<typename W, typename S, typename E>
	Node<W,S,E>::~Node() {
		//delete all Links (input and output)
		clear();
		//remove self from manager
		index->remove(ID);
	} //destructor
	
	//==================== METHODS =====================
	template<typename W, typename S, typename E>
	void Node<W,S,E>::update_output(Link<W,S,E>* const oldLink, Link<W,S,E>* const newLink) {
		//updates output Link pointer
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( *it == oldLink ) {
				*it = newLink;
				break;
			}
			++it;
		}
	}
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_input(	const unsigned int origin,
					const W& weight) {
		//Link will create its own pointer at the origin Node
		inputs.push_back( Link<W,S,E>(index, origin, ID, weight) );
	} //add_input
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_input(const unsigned int origin) {
		//destroys input Link to the specified Node
		auto it = inputs.begin();
		auto ite = inputs.end();
		while(it != ite) {
			if(it->origin == origin) {
				inputs.erase(it);
				break;
			}
			++it;
		}
	} //remove_input
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::add_output(Link<W,S,E>* const pLink) {
		//private; for adding output Link pointer
		outputs.push_back(pLink);
	} //add_output
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::remove_output(Link<W,S,E>* const pLink) {
		//removes output Link pointer
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			if( *it == pLink ) {
				outputs.erase(it);
				break;
			}
			++it;
		}
	} //remove_output
	
	template<typename W, typename S, typename E>
	void Node<W,S,E>::clear() {
		//delete all outputs from other nodes (where they are inputs)
		auto it = outputs.begin();
		auto ite = outputs.end();
		while(it != ite) {
			index->find( (*it)->target )->remove_input(ID);
			++it;
		}
		
		//remove all inputs from other nodes (where they are outputs)
		inputs.clear();
	} //clear
	
} //namespace ben

#endif
