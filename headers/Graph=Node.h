#ifndef GraphNode_h
#define GraphNode_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
*/

#include <iostream>
#include <map>
#include <vector>
#include <memory>

namespace Graph {
	
	template<typename T, typename S, typename E>
	class Index;
	
	template<typename T, typename S, typename E>
	class Node {
	
	private:
		//logistics
		static unsigned int smnIDCOUNT;
		inline static unsigned int getNewID()
			{ return smnIDCount++; }
		weak_ptr<Index<T,S,E>> mpIndex;
		T mtBias;
		inline weak_ptr<Node> find(const unsigned int nAddress) const {
			if(mpIndex) return mpIndex->find(nAddress);
			else return sINDEX->find(nAddress);
		}
		
		//hidden copy functionality
		Node& operator=(const Node& cSource);		//hidden assignment operator
		Node(const Node& cSource); 					//hidden copy constructor, maybe not hide this?
		
		//connection struct
		//distinguish between I/O Connections?
		struct Connection {
			using namespace std;
		private:
			Connection();
		protected:
			Connection( const weak_ptr<Node> pTarget,
						const shared_ptr< deque<S> > pSignal,
						const shared_ptr< deque<E> > pError,
						const T tWeight,
						const unsigned int nDelay)
				: target(pTarget), signal(pSignal), error(pError), weight(tWeight), delay(nDelay) {}
		public:
			weak_ptr<Node> target;
			shared_ptr< deque<S> > signal;
			shared_ptr< deque<E> > error;
			T weight;
			unsigned int delay;
		
			//input connection access
			inline bool signalReady() { return signal->size() == delay+1; }
			inline void push(E& eIn) { 
				if( error->size() > delay ) throw "Error already full";
				error->push_back(eIn); }
			inline void pull(S& sOut) {
				if( signal->size() <= delay ) throw "Signal not ready";
				sOut = signal->front();
				signal->pop_front(); }
			
			//output connection access
			inline bool errorReady() { return error->size() == delay+1; }
			inline void push(S& sIn) { 
				if( signal->size() > delay ) throw "Signal already full";
				signal->push_back(sIn); }
			inline void pull(E& eOut) {
				if( error->size() <= delay ) throw "Error not ready";
				eOut = error->front();
				error->pop_front(); }
		}; //struct Connection
		
		class Input_Connection : public Connection {
			using namespace std;
		private:
			inline bool errorReady();
			inline void push(S& sIn);
			inline void pull(E& eOut);
		public:
			Input_Connection(const weak_ptr<Node> pTarget,
							 const shared_ptr< deque<S> > pSignal,
							 const shared_ptr< deque<E> > pError,
							 const T tWeight,
							 const unsigned int nDelay)
				: Connection(pTarget, pSignal, pError, tWeight, nDelay) {}
		}; //class Input_Connection
		
		class Output_Connection : public Connection {
			using namespace std;
		private:
			inline bool signalReady();
			inline void push(E& eIn);
			inline void pull(S& sOut);
		public:
			Output_Connection(const weak_ptr<Node> pTarget,
							  const shared_ptr< deque<S> > pSignal,
							  const shared_ptr< deque<E> > pError,
							  const T tWeight,
							  const unsigned int nDelay)
				: Connection(pTarget, pSignal, pError, tWeight, nDelay) {}
		}; //class Output_Connection
		
		//connection storage
		vector<Input_Connection> mvInputs;
		vector<Output_Connection> mvOutputs;
		
	public: 
		//Node ID and indexing
		static Index sINDEX;
		const unsigned int ID;
	
		//constructors, destructor
		Node(); //<-------------not finished
		~Node(); //<-------------not finished
	
		//----------- connection management ---------------
		Node& addInput( const unsigned int nNewIn,
						const unsigned T tWeight
						const unsigned nTimeDelay=0);
		Node& addOutput(const unsigned int nNewOut,
						const unsigned T tWeight
						const unsigned nTimeDelay=0);
		Node& removeInput(const unsigned int nOldIn);
		Node& removeOutput(const unsigned int nOldOut);
	
		//-------------iterator---------------
		class iterator {
		private:
			Connection* mpCurrent;
		
		public:
			//////// constructors, assignment /////////
			iterator()
				: mpCurrent(NULL) {}
			iterator(const Connection* pCurrent)
				: mpCurrent(pCurrent) {}
			iterator(const iterator& iOld) 
				: mpCurrent(iOld.mpCurrent) {}
			inline iterator& operator=(const iterator& iRhs) {
				if( this != &iRhs ) mpCurrent = iRhs.mpCurrent;
				return *this; }
			
			//////// overloaded operators ///////////
			//pointer reference/dereference
			inline Connection& operator*() { 
				//if(mpCurrent==NULL) throw "Dereferenced null iterator";
				//else 
					return *mpCurrent; }
			inline Connection* operator->() {
				//if(mpCurrent==NULL) throw "Dereferenced null iterator";
				//else 
					return mpCurrent; }
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
			
			//pointer arithmetic
			inline iterator& operator++() { ++mpCurrent; }
			inline iterator& operator--() { --mpCurrent; }
			iterator operator++(int); //delegates to prefix version
			iterator operator--(int); //delegates to prefix version
			inline iterator& operator+=(const int nIndex) { 
				mpCurrent += nIndex; 
				return *this; }
			inline iterator& operator-=(const int nIndex) {
				mpCurrent -= nIndex;
				return *this; }
			inline const iterator operator+(const int nIndex) //delegates to operator+=
				{ iterator iNew(*this) += iRhs; }
			inline const iterator operator-(const int nIndex) //delegates to operator-=
				{ iterator iNew(*this) -= iRhs; }
		}; //class iterator
		
		struct input_iterator : public iterator {
			friend input_iterator& operator>>(input_iterator& out, S& sSignal); //delegates to Connection::pull(S&)
			friend input_iterator& operator<<(input_iterator& in, E& eError); //delegates to Connection::push(E&)
		}; //class input_iterator
		
		struct output_iterator : public iterator {
			friend output_iterator& operator<<(output_iterator& out, S& sSignal); //delegates to Connection::push(S&)
			friend output_iterator& operator>>(output_iterator& in, E& eError); //delegates to Connection::pull(E&)
		}; //class output_iterator
	
		//--------- iterator-related methods ---------------
		input_iterator input_begin(); 
		input_iterator input_end(); 
		output_iterator output_begin(); 
		output_iterator output_end(); 
	}; //class Node

	#include "Graph=Node.cpp"

} //namespace Graph

#endif
