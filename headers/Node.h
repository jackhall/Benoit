#ifndef BenoitNode_h
#define BenoitNode_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <iostream>
#include <list>
#include <vector>

namespace ben {
	
	//forward declarations for port streaming operators
	template<typename W, typename S>
	typename Node<W,S>::input_port& operator>>(typename Node<W,S>::input_port& in, S& signal);
	
	template<typename W, typename S>
	typename Node<W,S>::output_port& operator<<(typename Node<W,S>::output_port& out, S& signal);
	////////////////////////////
	
	template<typename W, typename S> 
	class Node {
	private:
		static unsigned int IDCOUNT;
		inline static unsigned int getNewID() { return IDCOUNT++; }
	
		std::list< Link<W,S> > inputs; //FIELD
		std::vector< Link<W,S>* > outputs; //FIELD
		Index<W,S>* index; //FIELD
		
		friend Index<W,S>; //managing Index needs to update its pointer
		
		friend Link<W,S>::Link(Index<W,S>* const pIndex,
		     			 const unsigned int nOrigin, 
		     			 const unsigned int nTarget, 
		     			 const W& wWeight);
		friend Link<W,S>::~Link(); //Link ctor and dtor need to manage their pointers
		void update_output(Link<W,S>* const oldLink, Link<W,S>* const newLink);
		void remove_output(Link<W,S>* const pLink);
		void add_output(Link<W,S>* const pLink);
	
	public:
		const unsigned int ID;
		W bias;
		
		Node();
		Node(Index<W,S>* const pIndex);
		//copy constructor, assignment operator?
		Node(const Node& rhs); //duplicates all input links but no output links
		Node& operator=(const Node& rhs); //like copy ctor, but first clears all previous links
		~Node(); 
	
		void add_input(	const unsigned int nOrigin,
				const W& wWeight);
		void remove_input(const unsigned int nOrigin);
		void clear();
		bool contains_input(const unsigned int nOrigin) const;
		bool contains_output(const unsigned int nTarget) const;
	
		class input_port {	
		private:
			typename std::list< Link<W,S> >::iterator current;

			friend class Node;
			input_port(const typename std::list< Link<W,S> >::iterator iLink);
			
		public:
			input_port() = default;
			input_port(const input_port& rhs) = default;
			input_port& operator=(const input_port& rhs) = default;
			~input_port() = default;
			
			Link<W,S>& operator*() const { return *current; }
			Link<W,S>* operator->() const { return &*current; }
			
			input_port& operator++();
			input_port  operator++(int);
			input_port& operator--();
			input_port  operator--(int);
			
			bool operator==(const input_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const input_port& rhs) const
				{ return current!=rhs.current; }
				
			friend input_port& operator>> <W,S>(input_port& out, S& signal);		
		}; //class input_port
	
		class output_port {
		private:
			typename std::vector< Link<W,S>* >::iterator current;
			
			friend class Node;
			output_port(const typename std::vector< Link<W,S>* >::iterator iLink);
			
		public:
			output_port() = default;
			output_port(const output_port& rhs) = default;
			output_port& operator=(const output_port& rhs) = default;
			~output_port() = default;
			
			Link<W,S>& operator*() const { return **current; }
			Link<W,S>* operator->() const { return *current; }
			
			output_port& operator++();
			output_port  operator++(int);
			output_port& operator--();
			output_port  operator--(int);
			
			bool operator==(const output_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const output_port& rhs) const
				{ return current!=rhs.current; } 
				
			friend output_port& operator<< <W,S>(output_port& out, S& signal);
		}; //class output_port
		
		input_port  input_begin()  { return input_port( inputs.begin() ); }
		input_port  input_end()    { return input_port( inputs.end() ); }
		output_port output_begin() { return output_port( outputs.begin() ); }
		output_port output_end()   { return output_port( outputs.end() ); }
		
	}; //class Node
	
	//initialize static members
	template<typename W, typename S>
	unsigned int Node<W,S>::IDCOUNT = 0;
	
} //namespace ben

#endif
