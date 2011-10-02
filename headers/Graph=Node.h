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

	template<typename T, typename S, typename E>
	class Node {
	/*
		- Each Node is identified by a unique unsigned int. These ints are associated with pointers
			to each Node via a static STL map. A read-only access function thus allows a programmer to 
			address Nodes by number as well as by pointer (run-independence). 
		- There are method interfaces for adding and removing connections by Node pointer or number. 
			This is necessary because constructors and destructors must be called for the connections. 
	*/

	private:
		//static ID members (special handling for template classes?)
		static unsigned int snID_COUNT;	
		static map<unsigned int, Node*> smID_MAP; 	//one centralized copy avoids sychronization issues
		static unsigned int getNewID() { return snID_COUNT++; }
	
		//hidden copy functionality
		Node& operator=(const Node& cSource);		//hidden assignment operator
		Node(const Node& cSource); 					//hidden copy constructor, maybe not hide this?
		
		//----------- connection class ------------
		class Connection {
		private:
			Node* mpTarget;
			S* mpSignalBuffer;
			S* mpCurrentSignal;
			E* mpErrorBuffer;
			E* mpCurrentError;
			T mtWeight;
			unsigned int mnDelay;
			
			Connection( const Node* pTarget,
						const S* pSignalBuffer,
						const E* pErrorBuffer,
						const T tWeight,
						unsigned int nDelay)
				: mpTarget(pTarget), mpSignalBuffer(pSignalBuffer),
					mpErrorBuffer(pErrorBuffer), mtWeight(tWeight),
					mnDelay(nDelay) {}
			
		public:		
			Connection()
				: pTarget(NULL), pSignalBuffer(NULL), 
					pErrorBuffer(NULL), tWeight(0.0), nDelay(0) {}
			Connection( const Node* pTarget,
						const nDelay=0,
						const tWeight=0.0); //nd
			~Connection(); //nd
			
			void pushSignal(S sSignal); //nd
			S pullSignal(); //nd
			void pushError(E eError); //nd
			E pullError(); //nd
			
			friend Connection& operator<<(Connection& out, S& sSignal); //delegates to pushSignal
			friend Connection& operator<<(Connection& out, E& eError); //delegates to pushError
			friend Connection& operator>>(Connection& in, S& sSignal); //delegates to pullSignal
			friend Connection& operator>>(Connection& in, E& eError); //delegates to pullError
		};
		
		//connection storage
		vector<Connection> mvInputs;
		vector<Connection> mvOutputs;
		
	public: 
		//Node ID and indexing
		const unsigned int ID;
		static inline Node* find(const unsigned int nAddress) //access a Node via its integer ID
			{ return smID_MAP[nAddress]; }
	
		//constructor, destructor
		Node(); //nd
		~Node(); //nd
	
		//----------- connection management ---------------
		Node& addInput( const No* pNewIn,
						const unsigned nTimeDelay=0); //not finished?
		Node& addInput( const unsigned int nNewIn,		//delegates to addInput(const Node*...
						const unsigned nTimeDelay=0);
		Node& addOutput(const Node* pNewOut,
						const unsigned nTimeDelay=0); //not finished?
		Node& addOutput(const unsigned int nNewOut,		//delegates to addOutput(const Node*...
						const unsigned nTimeDelay=0); 
		Node& removeInput(Node* pOldIn); //nd
		Node& removeInput(const unsigned int nOldIn);	//delegates to removeInput(const Node*...
		Node& removeOutput(Node* pOldOut); //nd
		Node& removeOutput(const unsigned int nOldOut);	//delegates to removeOutput(const Node*...
	
		//-------------iterator---------------
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
			iterator(const Connection* pFirst,
					 const Connection* pLast,
					 const Connection* pCurrent)
				: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbPastEnd(false) {}
			iterator(const iterator& iOld) 
				: mpFirst(iOld.mpFirst), mpLast(iOld.mpLast), mpCurrent(iOld.mpCurrent),
					mbPastEnd(iOld.mbPastEnd) {}
			iterator& operator=(const iterator& iRhs); 
			
			//////// miscellaneous methods
			inline bool inBounds() { return mpCurrent!=NULL; } 
			inline bool outofBounds() { return mpCurrent==NULL; } 
			
			inline void pushSignal(const S sSignal) 
				{ mpCurrent->pushSignal(sSignal); }
			inline S pullSignal() const
				{ return mpCurrent->pullSignal(); }
			inline void pushError(const E eError);
				{ mpCurrent->pushError(eError); }
			inline E pullError() const
				{ return mpCurrent->pullError(); }
			
			//////// overloaded operators ///////////
			//pointer reference/dereference
			inline Connection& operator*() { 
				if(mpCurrent==NULL) throw "Dereferenced iterator out of bounds";
				else return *mpCurrent; }
			inline Connection* operator->() {
				if(mpCurrent==NULL) throw "Dereferenced iterator out of bounds";
				else return mpCurrent; }
			Connection& operator[](const int nIndex) //delegates to operator+=
				{ return *(*this += nIndex); }
			
			//comparisons
			inline bool operator==(const iterator& cTwo) //similar to operator!=
				{ return mpCurrent==cTwo.mpCurrent; }
			inline bool operator!=(const iterator& cTwo) //similar to operator==
				{ return mpCurrent!=cTwo.mpCurrent; } 
			inline bool operator>(const iterator& iRhs)
				{ return mpCurrent>iRhs.mpCurrent; }
			inline bool operator<(const iterator& iRhs) 
				{ return mpCurrent<iRhs.mpCurrent; }
			inline bool operator>=(const iterator& iRhs) //delegates to operator<
				{ return !(mpCurrent<iRhs.mpCurrent); }
			inline bool operator<=(const iterator& iRhs) //delegates to operator>
				{ return !(mpCurrent>iRhs.mpCurrent); }
			
			//I/O streaming
			friend iterator& operator<<(iterator& out, S& sSignal); //delegates to Connection::pushSignal
			friend iterator& operator<<(iterator& out, E& eError); //delegates to Connection::pushError
			friend iterator& operator>>(iterator& in, S& sSignal); //delegates to Connection::pullSignal
			friend iterator& operator>>(iterator& in, E& eSignal); //delegates to Connection::pullError
			
			//pointer arithmetic
			iterator& operator++();
			iterator& operator--();
			iterator operator++(int); //delegates to prefix version
			iterator operator--(int); //delegates to prefix version
			iterator& operator+=(const int nIndex);
			iterator& operator-=(const int nIndex);
			inline const iterator operator+(const int nIndex) //delegates to operator+=
				{ iterator iNew(*this) += iRhs; }
			inline const iterator operator-(const int nIndex) //delegates to operator-=
				{ iterator iNew(*this) -= iRhs; }
		}; //class iterator
	
		//--------- iterator-related methods ---------------
		iterator inputBegin(); //nd
		iterator inputEnd(); //nd
		iterator outputBegin(); //nd
		iterator outputEnd(); //nd
	}; //class Node

	#include "Graph=Node.cpp"

} //namespace Graph

#endif
