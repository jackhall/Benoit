#ifndef BenoitIndex_h
//Node header needs to see members of Index
#include "Graph=Index.h"

#else
#ifndef BenoitNode_h
#define BenoitNode_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
*/

#include <iostream>
#include <map>
#include <vector>
#include <memory>

namespace Graph {
	
	template<typename T, typename S, typename E>
	class Node {
	
	private:
		//logistics
		static unsigned int smnIDCOUNT;
		inline static unsigned int getNewID()
			{ return smnIDCount++; }
		Index<T,S,E>* mpIndex; //FIELD
		T mtBias; //FIELD
		inline weak_ptr<Node> find(const unsigned int nAddress) const {
			if(mpIndex) return mpIndex->find(nAddress);
			else return sINDEX->find(nAddress);
		}
		void disconnectInput(shared_ptr<Node> pTarget);
		void disconnectOutput(shared_ptr<Node> pTarget);
		~Node(); 
		friend void Index<T,S,E>::erase;
		
		//hidden constructors (called by Index methods)
		Node(); //undefined, not used by Index so far
		Node(const T tBias, const Index<T,S,E>* pIndex);
		Node(const Node& cSource); //undefined, not used by Index so far
		Node& operator=(const Node& cSource); //undefined, not used by Index so far
		
		//================= connection base struct ================
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
		
		//================ Input_Connection =================
		struct Input_Connection : public Connection {
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
		
		//================= Output_Connection ==================
		struct Output_Connection : public Connection {
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
		
		//================ connection storage =====================
		vector<Input_Connection> mvInputs; //FIELD
		vector<Output_Connection> mvOutputs; //FIELD
	
	/////////////////////////////////////////////////////////////////////////
	public: 
		//Node ID and indexing
		static Index sINDEX;
		const unsigned int ID; //FIELD
		void disconnectAll(); 
	
		//=============== connection management =====================
		Node& addInput( const unsigned int nNewIn,
						const unsigned T tWeight
						const unsigned int nTimeDelay=0);
		Node& addOutput(const unsigned int nNewOut,
						const unsigned T tWeight
						const unsigned int nTimeDelay=0);
		Node& removeInput(const unsigned int nOldIn);
		Node& removeOutput(const unsigned int nOldOut);
		
		//================ iterator base (template) ====================
		template<typename C>
		class iterator {
		protected:
			C* mpCurrent;
			
			//////// constructors /////////
			iterator()
				: mpCurrent(NULL) {}
			iterator(const C* pCurrent)
				: mpCurrent(pCurrent) {}
			iterator(const iterator& iOld) 
				: mpCurrent(iOld.mpCurrent) {}
				
		public:
			//indirection				
			inline C& operator*() { 
				//if(mpCurrent==NULL) throw "Dereferenced null iterator";
				//else 
					return *mpCurrent; }
			inline C* operator->() {
				//if(mpCurrent==NULL) throw "Dereferenced null iterator";
				//else 
					return mpCurrent; }
			C& operator[](const int nIndex) //delegates to operator+=
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
			
		}; //class iterator
		
		//=================== input_iterator ============================
		struct input_iterator : iterator<Input_Connection> {	
			//constructors
			input_iterator()
				: iterator<Input_Connection>() {}
			input_iterator(const Input_Connection* pCurrent)
				: iterator<Input_Connection<(pCurrent) {}
			input_iterator(const input_iterator& iOld)
				: iterator<Input_Connection>(iOld) {}
			inline input_iterator& operator=(const input_iterator& iRhs) {
				if( this != &iRhs ) mpCurrent = iRhs.mpCurrent;
				return *this; } 
			
			//pointer arithmetic
			inline input_iterator& operator++() { ++mpCurrent; }
			inline input_iterator& operator--() { --mpCurrent; }
			input_iterator operator++(int); //delegates to prefix version
			input_iterator operator--(int); //delegates to prefix version
			inline input_iterator& operator+=(const int nIndex) { 
				mpCurrent += nIndex; 
				return *this; }
			inline input_iterator& operator-=(const int nIndex) {
				mpCurrent -= nIndex;
				return *this; }
			inline const input_iterator operator+(const int nIndex) //delegates to operator+=
				{ input_iterator iNew(*this) += iRhs; }
			inline const input_iterator operator-(const int nIndex) //delegates to operator-=
				{ input_iterator iNew(*this) -= iRhs; }
				
			//streaming operators
			friend input_iterator& operator>>(input_iterator& out, S& sSignal); //delegates to Connection::pull(S&)
			friend input_iterator& operator<<(input_iterator& in, E& eError); //delegates to Connection::push(E&)
			
		}; //class input_iterator
		
		//================== output_iterator =======================
		struct output_iterator : public iterator<Output_Connection> {
			//constructors
			output_iterator()
				: iterator<Output_Connection>() {}
			output_iterator(const Output_Connection* pCurrent)
				: iterator<Output_Connection<(pCurrent) {}
			output_iterator(const output_iterator& iOld)
				: iterator<Output_Connection>(iOld) {}
			inline output_iterator& operator=(const output_iterator& iRhs) {
				if( this != &iRhs ) mpCurrent = iRhs.mpCurrent;
				return *this; } 
			
			//pointer arithmetic
			inline output_iterator& operator++() { ++mpCurrent; }
			inline output_iterator& operator--() { --mpCurrent; }
			output_iterator operator++(int); //delegates to prefix version
			output_iterator operator--(int); //delegates to prefix version
			inline output_iterator& operator+=(const int nIndex) { 
				mpCurrent += nIndex; 
				return *this; }
			inline output_iterator& operator-=(const int nIndex) {
				mpCurrent -= nIndex;
				return *this; }
			inline const output_iterator operator+(const int nIndex) //delegates to operator+=
				{ output_iterator iNew(*this) += iRhs; }
			inline const output_iterator operator-(const int nIndex) //delegates to operator-=
				{ output_iterator iNew(*this) -= iRhs; }
				
			//streaming operators
			friend output_iterator& operator<<(output_iterator& out, S& sSignal); //delegates to Connection::push(S&)
			friend output_iterator& operator>>(output_iterator& in, E& eError); //delegates to Connection::pull(E&)
			
		}; //class output_iterator
	
		//================ iterator generation ======================
		inline input_iterator input_begin() { return input_iterator( &mvInputs.front() ); }
		inline input_iterator input_end() { return input_iterator( &mvInputs.back()+1 ); }
		inline output_iterator output_begin() { return output_iterator( &mvOutputs.front() ); }
		inline output_iterator output_end() { return output_iterator( &mvOutputs.back()+1 ); }
		
	}; //class Node

	#include "Graph=Node.cpp"

} //namespace Graph

#endif //ifndef GraphNode_h
#endif //ifndef GraphIndex_h
