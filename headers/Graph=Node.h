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
#include <memory>
#include "Graph=Index.h"

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
		//logistics
		shared_ptr<Index<T,S,E>> mpIndex;
		weak_ptr<Node> mpSelf;
		T tBias;
		
		//hidden copy functionality
		Node& operator=(const Node& cSource);		//hidden assignment operator
		Node(const Node& cSource); 					//hidden copy constructor, maybe not hide this?
		
		//connection struct
		struct Connection {
			using namespace std;
			weak_ptr<Node> target;
			shared_ptr<S> signal;
			shared_ptr<E> error;
			T weight;
			unsigned int delay;
			Connection( const weak_ptr<Node> pTarget, 
						const shared_ptr<S> pSignal, 
						const shared_ptr<E> pError,
						const T tWeight,
						const unsigned int nDelay)
				: target(pTarget), signal(pSignal), error(pError), weight(tWeight), delay(nDelay) {}
		};
		
		//connection storage
		vector<Connection> mvInputs;
		vector<Connection> mvOutputs;
		
	public: 
		//Node ID and indexing
		const unsigned int ID;
	
		//constructor, destructor
		Node(); //nd
		~Node(); //nd
	
		//----------- connection management ---------------
		Node& addInput( const unsigned int nNewIn,
						const unsigned T tWeight
						const unsigned nTimeDelay=0);
		Node& addOutput(const unsigned int nNewOut,
						const unsigned T tWeight
						const unsigned nTimeDelay=0);
		Node& removeInput(const unsigned int nOldIn);
		Node& removeOutput(const unsigned int nOldOut);
		
		Connection newConnection(const Node* pNew, const unsigned int nDelay); 
	
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
		iterator inputBegin(); 
		iterator inputEnd(); 
		iterator outputBegin(); 
		iterator outputEnd(); 
	}; //class Node

	#include "Graph=Node.cpp"

} //namespace Graph

#endif
