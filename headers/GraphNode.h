#ifndef GraphNode_h
#define GraphNode_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	A Node is the basic independent causal unit in a neural network. 
	The concept is similar to a directed graph in mathematics. The execution 
	of a Node does not depend on any single other part of the network 
	(special cases aside), but on many. A Node can be a Neuron, a group of 
	neurons independent of each other (Ganglion), or an entire network (NeuralNet). 
	This allows for a recursive hierarchy of Nodes. 
*/

#include <iostream>
#include <map>
#include <vector>

using namespace std;

template<typename T, typename U, typename W>
class Node {
/*
	- Each Node is identified by a unique unsigned int. These ints are associated with pointers
		to each Node via a static STL map. A read-only access function thus allows a programmer to 
		address Nodes by number as well as by pointer (run-independence). 
	- There are method interfaces for adding and removing connections by Node pointer or number. 
		This is necessary because constructors and destructors must be called for the connections. 
*/

private:
	//connection lists
	vector<GraphNode*> mvInputConnections;
	vector<GraphNode*> mvOutputConnections;
	
	//static ID members
	static unsigned int snID_COUNT;	
	static map<unsigned int, GraphNode*> smID_MAP; 	//one centralized copy avoids sychronization issues
	static unsigned int getNewID() { return snID_COUNT++; }
	
	GraphNode& operator=(const GraphNode& cSource);		//hidden assignment operator
	GraphNode(const GraphNode& cSource); 					//hidden copy constructor

protected:
	virtual ostream& print(ostream &out) const; //not finished

public: 
	//Node ID and indexing
	const unsigned int ID;
	static inline GraphNode* find(const unsigned int nAddress) //access a Node via its integer ID
		{ return smID_MAP[nAddress]; }
	
	//constructor, destructor
	GraphNode();
	virtual ~GraphNode() {}
	
	//connection management
	GraphNode& addInput(const GraphNode* pNewIn,
						const bool bTimeDelay=false); 
	GraphNode& addInput(const unsigned int nNewIn,		//delegates to previous
						const bool bTimeDelay=false);
	GraphNode& addOutput(const GraphNode* pNewOut,
						 const bool bTimeDelay=false); 
	GraphNode& addOutput(const unsigned int nNewOut,		//delegates to previous
						const bool bTimeDelay=false);
	GraphNode& removeInput(GraphNode* pOldIn);
	GraphNode& removeInput(const unsigned int nOldIn);	//delegates to previous
	GraphNode& removeOutput(GraphNode* pOldOut);
	GraphNode& removeOutput(const unsigned int nOldOut);	//delegates to previous
	
	//iterators
	class iterator {
	private:
		GraphNode* mpFirst; //only works if elements of GraphNode storage are serial
		GraphNode* mpLast;
		GraphNode* mpCurrent;
		bool mbPastEnd;
		
	public
		iterator()
			: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbPastEnd(false) {}
		iterator( 	const GraphNode* pFirst,
					const GraphNode* pLast,
					const GraphNode* pCurrent)
			: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbPastEnd(false) {}
		iterator(const iterator& iOld) 
			: mpFirst(iOld.mpFirst), mpLast(iOld.mpLast), mpCurrent(iOld.mpCurrent),
				mbPastEnd(iOld.mbPastEnd) {}
		iterator&  operator=(const iterator& iRhs);
		inline bool inBounds() { return mpCurrent!=NULL; }
		inline bool outofBounds() { return mpCurrent==NULL; }
		inline GraphNode& operator*() { 
			if(mpCurrent==NULL) throw "Dereferenced input_iterator out of bounds";
			else return *mpCurrent; }
		input_iterator& operator++();
		input_iterator& operator--();
		input_iterator& operator++(int);
		input_iterator& operator--(int);
		bool operator==(const input_iterator& cTwo) 
			{ return mpCurrent==cTwo.mpCurrent; }
		bool operator!=(const input_iterator& cTwo) 
			{ return mpCurrent!=cTwo.mpCurrent; }
		//pointer arithmetic
		//[] dereference
		//comparison > < => =<
		//compound assignment 
	};
	
	class input_iterator : public iterator {
	private:
	public:
	};
	
	class output_iterator : public iterator {
	private:
	public:
	};
	
	input_iterator inputBegin();
	input_iterator inputEnd();
	output_iterator outputBegin();
	output_iterator outputEnd();
};

#include "GraphNode.cpp"

#endif
