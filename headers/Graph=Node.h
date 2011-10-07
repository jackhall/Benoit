#ifndef GraphNode_h
#define GraphNode_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	
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
	*/
	
	private:
		//logistics
		static unsigned int smnIDCount;
		inline static unsigned int getNewID()
			{ return smnIDCount++; }
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
			unsigned int signalMarker;
			unsigned int errorMarker;
			
			Connection( const weak_ptr<Node> pTarget, 
						const shared_ptr<S> pSignal, 
						const shared_ptr<E> pError,
						const T tWeight,
						const unsigned int nDelay)
				: target(pTarget), signal(pSignal), error(pError), weight(tWeight), 
					delay(nDelay), signalMarker(0), errorMarker(0) {}
			
			void push(S& sIn);
			void push(E& eIn);
			S pullSignal();
			E pullError();
			inline unsigned int& step(unsigned int& marker)
				{ if(marker == delay-1) return marker = 0;
				  else return ++marker; }
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
				{ mpCurrent->push(sSignal); }
			inline S pullSignal() const
				{ return mpCurrent->pullSignal(); }
			inline void pushError(const E eError);
				{ mpCurrent->push(eError); }
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
			friend iterator& operator<<(iterator& out, S& sSignal); //delegates to Connection::push(S&)
			friend iterator& operator<<(iterator& out, E& eError); //delegates to Connection::push(E&)
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
