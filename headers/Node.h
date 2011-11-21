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
		void update_index(const Index<W,S,E>* pIndex);
		friend Index<W,S,E>; //manager class rights for move and swap
		
		void update_output(const Link<W,S,E>* oldLink, const Link<W,S,E>* newLink);
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
		Node(const Node& rhs); //should preserve uniqueness
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
		
		//=================== input_port ============================
		class input_port {	
		private:
			list< Link<W,S,E> >::iterator current;
			
			friend Node;
			input_port( list< Link<W,S,E> >::iterator iLink );
			
		public:
			//constructors
			input_port() = default;
			input_port(const input_port& rhs);
			input_port& operator=(const input_port& rhs);
			~input_port();
			
			//indirection, replace with more specific methods?
			Link<W,S,E>& operator*() const { return *current; }
			Link<W,S,E>* operator->() const { return &*current; }
			
			//pointer arithmetic
			input_port& operator++();
			input_port& operator--();
				
			//streaming operators
			friend input_port& operator>>(input_port& out, S& sSignal);
			friend input_port& operator<<(input_port& in, E& eError); 
			
			//comparisons
			bool operator==(const input_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const input_port& rhs) const
				{ return current!=rhs.current; } 
		}; //class input_port
		
		//================== output_port =======================
		class output_port {
		private:
			vector< Link<W,S,E>* >::iterator current;
			
			friend Node;
			output_port( vector< Link<W,S,E>* >::iterator iLink );
			
		public:
			//constructors
			output_port() = default;
			output_port(const output_port& rhs);
			output_port& operator=(const output_port& rhs);
			~output_port();
			
			//indirection, replace with more specific methods?
			Link<W,S,E>& operator*() const { return **current; }
			Link<W,S,E>* operator->() const { return *current; }
			
			//pointer arithmetic
			output_port& operator++();
			output_port& operator--();
				
			//streaming operators
			friend output_port& operator<<(output_port& out, S& sSignal); 
			friend output_port& operator>>(output_port& in, E& eError); 
			
			//comparisons
			bool operator==(const output_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const output_port& cTwo) const
				{ return current!=rhs.current; } 
		}; //class output_port
	
		//================ iterator generation ======================
		input_port  input_begin()  { return input_port( inputs.begin() ); }
		input_port  input_end()    { return input_port( inputs.end() ); }
		output_port output_begin() { return output_port( outputs.begin() ); }
		output_port output_end()   { return output_port( outputs.end() ); }
		
	}; //class Node

	unsigned int Node::IDCOUNT = 0; //initialize static member
} //namespace ben

#endif //ifndef GraphNode_h
#endif //ifndef GraphIndex_h
