#ifndef GraphNode_cpp
#define GraphNode_cpp
//(c) Jack Hall 2011, licensed under GNU LGPL v3

unsigned int GraphNode::snID_COUNT = 0; //initialize ID numbers

ostream& GraphNode::print(ostream &out) const {
	//more here?
	out << "ID# " << ID << endl;
	return out;
}

GraphNode& GraphNode::addInput(const unsigned int nNewIn,
					 			const char chOperator,
					 			const unsigned int nTimeDelay) {
	//look up Node ID
	GraphNode* pNewIn = find(nNewIn);
	//call a Connection_base-derived constructor
	return addInput(pNewIn, chOperator, nTimeDelay); //defined in child classes
}

GraphNode& GraphNode::addOutput(const unsigned int nNewOut,
								const char chOperator,
								const unsigned int nTimeDelay) {
	//look up Node ID
	GraphNode* pNewOut = find(nNewOut);
	//call a Connection_base-derived constructor
	return addOutput(pNewOut, chOperator, nTimeDelay); //defined in child classes
}

GraphNode& GraphNode::removeInput(const unsigned int nOldIn) {
	//look up Node ID
	GraphNode* pOldIn = find(nOldIn);
	//calls a Connection_base-derived destructor
	return removeInput(pOldIn); //defined in child classes
}

GraphNode& GraphNode::removeOutput(const unsigned int nOldOut) {
	//look up Node ID
	GraphNode* pOldOut = find(nOldOut);
	//calls a Connection_base-derived destructor
	return removeInput(pOldOut); //defined in child classes
}

#endif

