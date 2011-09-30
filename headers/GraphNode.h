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

namespace Graph {

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
		//connection storage
		vector<Connection> mvInputs;
		vector<Connection> mvOutputs;
	
		//static ID members
		static unsigned int snID_COUNT;	
		static map<unsigned int, Node*> smID_MAP; 	//one centralized copy avoids sychronization issues
		static unsigned int getNewID() { return snID_COUNT++; }
	
		Node& operator=(const Node& cSource);		//hidden assignment operator
		Node(const Node& cSource); 					//hidden copy constructor, maybe not hide this?
		
		//----------- connection class ------------
		class Connection {
		private:
			Node* mpTarget;
			unsigned int mnTarget;
			U* mpSignalBuffer;
			W* mpErrorBuffer;
			T mtWeight;
			unsigned int mnDelay;
			
		public:		
			Connection()
				: pTarget(NULL), pSignalBuffer(NULL), 
					pErrorBuffer(NULL), tWeight(0.0), nDelay(0) {}
			Connection( const Node* pTarget,
						const nDelay=0,
						const tWeight=0.0);
			
			void pushSignal(U uSignal);
			U pullSignal();
			void pushError(W wError);
			W pullError();
			//add streamlike functionality?
		};

	protected:
		virtual ostream& print(ostream &out) const; //not finished

	public: 
		//Node ID and indexing
		const unsigned int ID;
		static inline Node* find(const unsigned int nAddress) //access a Node via its integer ID
			{ return smID_MAP[nAddress]; }
	
		//constructor, destructor
		Node();
		virtual ~Node() {}
	
		//----------- connection management ---------------
		Node& addInput( const Node* pNewIn,
						const bool bTimeDelay=false); 
		Node& addInput( const unsigned int nNewIn,		//delegates to previous
						const bool bTimeDelay=false);
		Node& addOutput(const Node* pNewOut,
						const bool bTimeDelay=false); 
		Node& addOutput(const unsigned int nNewOut,		//delegates to previous
						const bool bTimeDelay=false);
		Node& removeInput(Node* pOldIn);
		Node& removeInput(const unsigned int nOldIn);	//delegates to previous
		Node& removeOutput(Node* pOldOut);
		Node& removeOutput(const unsigned int nOldOut);	//delegates to previous
	
		//-------------iterators---------------
		class iterator {
		private:
			Connection* mpFirst; //only works if elements of Graph::Node storage are serial
			Connection* mpLast;
			Connection* mpCurrent;
			bool mbPastEnd;
		
		public:
			//////// constructors, assignment /////////
			iterator()
				: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbPastEnd(false) {}
			iterator(const Node* pFirst,
					 const Node* pLast,
					 const Node* pCurrent)
				: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbPastEnd(false) {}
			iterator(const iterator& iOld) 
				: mpFirst(iOld.mpFirst), mpLast(iOld.mpLast), mpCurrent(iOld.mpCurrent),
					mbPastEnd(iOld.mbPastEnd) {}
			iterator& operator=(const iterator& iRhs);
			
			//////// miscellaneous methods
			inline bool inBounds() { return mpCurrent!=NULL; }
			inline bool outofBounds() { return mpCurrent==NULL; }
			void push(const X& xData);
			X& pull() const;
			
			//////// overloaded operators ///////////
			inline Connection& operator*() { 
				if(mpCurrent==NULL) throw "Dereferenced iterator out of bounds";
				else return *mpCurrent; }
			inline Connection* operator->() {
				if(mpCurrent==NULL) throw "Dereferenced iterator out of bounds";
				else return mpCurrent; }
			iterator& operator++();
			iterator& operator--();
			iterator& operator++(int);
			iterator& operator--(int);
			bool operator==(const iterator& cTwo) 
				{ return mpCurrent==cTwo.mpCurrent; }
			bool operator!=(const iterator& cTwo) 
				{ return mpCurrent!=cTwo.mpCurrent; }
			//pointer arithmetic
			//[] dereference
			//comparison > < => =<
			//compound assignment 
		}; //class iterator
	
		class input_iterator : public iterator {
		private:
		public:
			input_iterator& operator=(const input_iterator& iRhs);
		}; //class input_iterator
	
		class output_iterator : public iterator {
		private:
		public:
		}; //class output_iterator
	
		//--------- iterator-related methods ---------------
		input_iterator inputBegin();
		input_iterator inputEnd();
		output_iterator outputBegin();
		output_iterator outputEnd();
	}; //class Node

	#include "GraphNode.cpp"

} //namespace Graph

#endif
