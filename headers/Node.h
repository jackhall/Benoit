#ifndef BenoitIndex_h
//Node header needs to see members of Index
#include "Index.h"

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

namespace Benoit {
	
	template<typename T, typename W, typename S, typename E> 
	class Node {
	
	private:
		//logistics
		static unsigned int smnIDCOUNT;
		inline static unsigned int getNewID()
			{ return smnIDCount++; }
		Index<T,W,S,E>* mpIndex; //FIELD
		friend Index<T,W,S,E>; //manager class needs full rights
		T bias; //FIELD
		
		//void disconnect_input(const unsigned int nTarget);
		//void disconnect_output(const unsigned int nTarget);
		
		//only Index can destruct a Node
		~Node(); 
		
		//hidden constructors (called by Index methods)
		Node(); //undefined, not used by Index so far
		Node& operator=(const Node& cSource); //undefined, not used by Index so far
		
		//================ connection storage =====================
		map< unsigned int, Link<T,W,S,E> > inputs; //FIELD
		vector< Link<T,W,S,E>* > outputs; //FIELD
	
	/////////////////////////////////////////////////////////////////////////
	public: 
		//Node ID and indexing
		static Index INDEX;
		const unsigned int ID; //FIELD
		//void disconnectAll(); 
		
		//exposed constructors
		Node(const Node& cSource); //should preserve uniqueness
		Node(const T tBias, const Index<T,S,E>* pIndex);
	
		//=============== link management =====================
		Node& addInput( const unsigned int nNewIn,
				const unsigned W wWeight);
		Node& addOutput(const unsigned int nNewOut,
				const unsigned W wWeight);
		Node& removeInput(const unsigned int nOldIn);
		Node& removeOutput(const unsigned int nOldOut);
		
		//================ iterator base ====================
		class iterator {
		protected:
			typedef <Link<T,W,S,E>* ptr;
			typedef map< unsigned int, Link<T,W,S,E> >::iterator iter;
			
			ptr; //FIELD
			iter element; //FIELD
			
			//////// constructors /////////
			iterator()
				: current(NULL) {} //correct?
			iterator(iter iCurrent)
				: current(iCurrent) {}
			iterator(const iterator& iOld) 
				: current(iOld.current) {}
				
		public:
			//indirection				
			Link<T,W,S,E>& operator*() const { return *ptr; }
			Link<T,W,S,E>* operator->() const { return ptr.operator->(); }
			
			//increment, decrement
			iterator& operator++();
			iterator& operator--();
			iterator operator++(int);
			iterator operator--(int);
			
			//comparisons
			bool operator==(const iterator& cTwo) //similar to operator!=
				{ return mpCurrent==cTwo.mpCurrent; }
			bool operator!=(const iterator& cTwo) //similar to operator==
				{ return mpCurrent!=cTwo.mpCurrent; } 
			
		}; //class iterator
		
		//=================== input_iterator ============================
		struct input_iterator : iterator {	
			typedef map< unsigned int, Link<T,W,S,E> >::iterator iter;
			
			
			//constructors
			input_iterator() 
				: iterator() {}
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
		struct output_iterator : public iterator {
			typedef vector< Link<T,W,S,E>* >::iterator iter;
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

	#include "Node.cpp"

} //namespace Graph

#endif //ifndef GraphNode_h
#endif //ifndef GraphIndex_h
