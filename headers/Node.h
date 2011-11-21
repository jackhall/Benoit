#ifndef BenoitNode_h
#define BenoitNode_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <mutex>

namespace ben {
	
	//forward declarations for port streaming operators
	template<typename W, typename S, typename E>
	typename Node<W,S,E>::input_port& operator<<(typename Node<W,S,E>::input_port& out, E& eError);
	
	template<typename W, typename S, typename E>
	typename Node<W,S,E>::input_port& operator>>(typename Node<W,S,E>::input_port& in, S& sSignal);
	
	template<typename W, typename S, typename E>
	typename Node<W,S,E>::output_port& operator<<(typename Node<W,S,E>::output_port& out, S& sSignal);
	
	template<typename W, typename S, typename E>
	typename Node<W,S,E>::output_port& operator>>(typename Node<W,S,E>::output_port& in, E& eError);
	
	//begin class template declaration
	template<typename W, typename S, typename E> 
	class Node {
	
	private:
		//logistics
		static unsigned int IDCOUNT;
		inline static unsigned int getNewID() { return IDCOUNT++; }
		std::mutex m;
		Index<W,S,E>* index; //FIELD
		void update_index(const Index<W,S,E>* pIndex);
		
		friend class Index<W,S,E>; //manager class rights for move and swap
		
		void update_output(const Link<W,S,E>* oldLink, const Link<W,S,E>* newLink);
		void remove_output(const Link<W,S,E>* pLink);
		void add_output(const Link<W,S,E>* pLink);
		
		//================ connection storage =====================
		std::list< Link<W,S,E> > inputs; //FIELD
		std::vector< Link<W,S,E>* > outputs; //FIELD
	
	/////////////////////////////////////////////////////////////////////////
	public: 
		//Node ID and indexing
		static Index<W,S,E> INDEX; //static FIELD, default Index
		const unsigned int ID; //FIELD
		
		//============= constructors, destructor ===============
		Node();
		Node(const Node& rhs); //should preserve uniqueness
		Node(const Index<W,S,E>* pIndex);
		Node& operator=(const Node& cSource); //should preserve uniqueness
		~Node(); 
	
		//=============== link management =====================
		void add_input(	const unsigned int nOrigin,
				const W& wWeight);
		void remove_input(const unsigned int nOrigin);
		void clear();
		bool contains_input(const unsigned int nOrigin) const;
		bool contains_output(const unsigned int nTarget) const;
		
		//=================== input_port ============================
		class input_port {	
		private:
			typename std::list< Link<W,S,E> >::iterator current;

			friend class Node;
			input_port(const typename std::list< Link<W,S,E> >::iterator iLink);

			
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
			friend input_port& operator>> <W,S,E>(input_port& out, S& sSignal);
			friend input_port& operator<< <W,S,E>(input_port& in, E& eError); 
			
			//comparisons
			bool operator==(const input_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const input_port& rhs) const
				{ return current!=rhs.current; } 
		}; //class input_port
		
		//================== output_port =======================
		class output_port {
		private:
			typename std::vector< Link<W,S,E>* >::iterator current;
			
			friend class Node;
			output_port(const typename std::vector< Link<W,S,E>* >::iterator iLink);
			
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
			friend output_port& operator<< <W,S,E>(output_port& out, S& sSignal); 
			friend output_port& operator>> <W,S,E>(output_port& in, E& eError); 
			
			//comparisons
			bool operator==(const output_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const output_port& rhs) const
				{ return current!=rhs.current; } 
		}; //class output_port
	
		//================ iterator generation ======================
		input_port  input_begin()  { return input_port( inputs.begin() ); }
		input_port  input_end()    { return input_port( inputs.end() ); }
		output_port output_begin() { return output_port( outputs.begin() ); }
		output_port output_end()   { return output_port( outputs.end() ); }
		
	}; //class Node
	
	
	//initialize static members
	template<typename W, typename S, typename E>
	unsigned int Node<W,S,E>::IDCOUNT = 0;
	
	template<typename W, typename S, typename E>
	Index<W,S,E> Node<W,S,E>::INDEX = Index<W,S,E>(); 
	
} //namespace ben

#endif //ifndef BenoitNode_h
