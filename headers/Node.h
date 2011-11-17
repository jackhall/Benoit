#ifndef BenoitIndex_h
//Node header needs to see members of Index
#include "Link.h"

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
		friend Index<T,W,S,E>; //manager class needs full rights?
		T bias; //FIELD
		
		//void destroy_input(const unsigned int nTarget);
		void remove_output(const unsigned int nTarget);
		Node& add_output(const unsigned int nNewOut,
					const unsigned W wWeight);
		
		//================ connection storage =====================
		map< unsigned int, Link<W,S,E> > inputs; //FIELD
		vector< Link<W,S,E>* > outputs; //FIELD
	
	/////////////////////////////////////////////////////////////////////////
	public: 
		//Node ID and indexing
		static Index<T,W,S,E> INDEX;
		const unsigned int ID; //FIELD
		//void disconnectAll(); 
		
		//============= constructors, destructor ===============
		Node(); //undefined, not used by Index so far
		Node(const Node& cSource); //should preserve uniqueness
		Node(const T tBias, const Index<T,S,E>* pIndex);
		Node& operator=(const Node& cSource); //undefined
		~Node(); 
	
		//=============== link management =====================
		Node& add_input( const unsigned int nNewIn,
				const unsigned W wWeight);
		Node& remove_input(const unsigned int nOldIn);
		
		//=================== input_iterator ============================
		class input_iterator {	
		private:
			friend Node;
			map< unsigned int, Link<W,S,E> >::iterator current;
			
			input_iterator( map< unsigned int, Link<W,S,E> >::iterator iLink );
			
		public:
			//constructors
			input_iterator() {}
			input_iterator(const input_iterator& iOld) {}
			inline input_iterator& operator=(const input_iterator& iRhs) {
				if( this != &iRhs ) current = iRhs.current;
				return *this; } 
			
			Link<W,S,E>& operator*() const { return current->second; }
			Link<W,S,E>* operator->() const { return current.operator->(); }
			
			//pointer arithmetic
			inline input_iterator& operator++() { ++mpCurrent; }
			inline input_iterator& operator--() { --mpCurrent; }
				
			//streaming operators
			friend input_iterator& operator>>(input_iterator& out, S& sSignal);
			friend input_iterator& operator<<(input_iterator& in, E& eError); 
			
			//comparisons
			bool operator==(const input_iterator& cTwo) //similar to operator!=
				{ return mpCurrent==cTwo.mpCurrent; }
			bool operator!=(const input_iterator& cTwo) //similar to operator==
				{ return mpCurrent!=cTwo.mpCurrent; } 
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
			
			Link<W,S,E>& operator*() const { return *ptr; }
			Link<W,S,E>* operator->() const { return ptr.operator->(); }
			
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
			friend output_iterator& operator<<(output_iterator& out, S& sSignal); 
			friend output_iterator& operator>>(output_iterator& in, E& eError); 
			
			//comparisons
			bool operator==(const output_iterator& cTwo) //similar to operator!=
				{ return mpCurrent==cTwo.mpCurrent; }
			bool operator!=(const output_iterator& cTwo) //similar to operator==
				{ return mpCurrent!=cTwo.mpCurrent; } 
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
