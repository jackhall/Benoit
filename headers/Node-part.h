#ifndef BenoitNode_h
#define BenoitNode_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <iostream>
#include <list>
#include <vector>

namespace ben {

	template<typename W, typename S, typename E> 
	class Node {
	private:
		static unsigned int IDCOUNT;
		inline static unsigned int getNewID() { return IDCOUNT++; }
	
		std::list< Link<W,S,E> > inputs; //FIELD
		std::vector< Link<W,S,E>* > outputs; //FIELD
		Index<W,S,E>* index; //FIELD
		
		friend Link<W,S,E>::Link(Index<W,S,E>* const pIndex,
		     			 const unsigned int nOrigin, 
		     			 const unsigned int nTarget, 
		     			 const W& wWeight);
		friend Link<W,S,E>::~Link();
		void update_output(Link<W,S,E>* const oldLink, Link<W,S,E>* const newLink);
		void remove_output(Link<W,S,E>* const pLink);
		void add_output(Link<W,S,E>* const pLink);
	
	public:
		const unsigned int ID;
		
		Node();
		Node(Index<W,S,E>* const pIndex);
		~Node(); 
	
		void add_input(	const unsigned int nOrigin,
				const W& wWeight);
		void remove_input(const unsigned int nOrigin);
		void clear();
		bool contains_input(const unsigned int nOrigin) const;
		bool contains_output(const unsigned int nTarget) const;
	
		class input_port {	
			private:
				typename std::list< Link<W,S,E> >::iterator current;

				friend class Node;
				input_port(const typename std::list< Link<W,S,E> >::iterator iLink);
	
		}; //class input_port
	
		class output_port {
			private:
				typename std::vector< Link<W,S,E>* >::iterator current;
			
				friend class Node;
				output_port(const typename std::vector< Link<W,S,E>* >::iterator iLink);
			public:
			
		}; //class output_port
		
		input_port  input_begin()  { return input_port( inputs.begin() ); }
		input_port  input_end()    { return input_port( inputs.end() ); }
		output_port output_begin() { return output_port( outputs.begin() ); }
		output_port output_end()   { return output_port( outputs.end() ); }
		
	}; //class Node
	
	//initialize static members
	template<typename W, typename S, typename E>
	unsigned int Node<W,S,E>::IDCOUNT = 0;
	
} //namespace ben

#endif
