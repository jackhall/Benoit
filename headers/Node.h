#ifndef BenoitNode_h
#define BenoitNode_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <mutex>

namespace ben {
	
	template<typename W, typename S, typename E> 
	class Node {
	
	using namespace std;
	private:
		//logistics
		static unsigned int IDCOUNT;
		inline static unsigned int getNewID() { return IDCount++; }
		mutex m;
		Index<W,S,E>* pIndex; //FIELD
		friend Index<W,S,E>; //manager class rights for move, take and swap
		
		void private_remove_input(const unsigned int nOrigin);
		void remove_output(const Link<W,S,E>* pLink);
		void add_output(const Link<W,S,E>* pLink);
		
		//================ connection storage =====================
		list< Link<W,S,E> > inputs; //FIELD
		vector< Link<W,S,E>* > outputs; //FIELD
	
	/////////////////////////////////////////////////////////////////////////
	public: 
		//Node ID and indexing
		static Index<W,S,E> INDEX; //static FIELD, default Index
		const unsigned int ID; //FIELD
		
		//============= constructors, destructor ===============
		Node() = delete;
		Node(const Node& cSource); //should preserve uniqueness
		Node(const Index<W,S,E>* pIndex=&INDEX);
		Node& operator=(const Node& cSource); //should preserve uniqueness
		~Node(); 
	
		//=============== link management =====================
		void add_input(	const unsigned int nOrigin,
				const unsigned W& wWeight);
		void remove_input(const unsigned int nOrigin);
		void clear();
		bool contains_input(const unsigned int nOrigin) const;
		bool contains_output(const unsigned int nTarget) const;
		
		//=================== input_iterator ============================
		class input_iterator {	
		private:
			list< Link<W,S,E> >::iterator current;
			
			friend Node;
			input_iterator( list< Link<W,S,E> >::iterator iLink );
			
		public:
			//constructors
			input_iterator() = default;
			input_iterator(const input_iterator& rhs);
			input_iterator& operator=(const input_iterator& rhs);
			~input_iterator();
			
			//indirection
			Link<W,S,E>& operator*() const;
			Link<W,S,E>* operator->() const;
			
			//pointer arithmetic
			input_iterator& operator++();
			input_iterator& operator--();
				
			//streaming operators
			friend input_iterator& operator>>(input_iterator& out, S& sSignal);
			friend input_iterator& operator<<(input_iterator& in, E& eError); 
			
			//comparisons
			bool operator==(const input_iterator& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const input_iterator& rhs) const
				{ return current!=rhs.current; } 
		}; //class input_iterator
		
		//================== output_iterator =======================
		class output_iterator {
		private:
			vector< Link<W,S,E>* >::iterator current;
			
			friend Node;
			output_iterator( vector< Link<W,S,E>* >::iterator iLink );
			
		public:
			//constructors
			output_iterator() = default;
			output_iterator(const output_iterator& rhs);
			output_iterator& operator=(const output_iterator& rhs);
			~output_iterator();
			
			//indirection
			Link<W,S,E>& operator*() const;
			Link<W,S,E>* operator->() const;
			
			//pointer arithmetic
			output_iterator& operator++();
			output_iterator& operator--();
				
			//streaming operators
			friend output_iterator& operator<<(output_iterator& out, S& sSignal); 
			friend output_iterator& operator>>(output_iterator& in, E& eError); 
			
			//comparisons
			bool operator==(const output_iterator& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const output_iterator& cTwo) const
				{ return current!=rhs.current; } 
		}; //class output_iterator
	
		//================ iterator generation ======================
		input_iterator  input_begin()  { return input_iterator( inputs.begin() ); }
		input_iterator  input_end()    { return input_iterator( inputs.end() ); }
		output_iterator output_begin() { return output_iterator( outputs.begin() ); }
		output_iterator output_end()   { return output_iterator( outputs.end() ); }
		
	}; //class Node

	unsigned int Node::IDCOUNT = 0; //initialize static member
} //namespace ben

#endif //ifndef GraphNode_h
#endif //ifndef GraphIndex_h
