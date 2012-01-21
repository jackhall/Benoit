#ifndef BenoitNode_h
#define BenoitNode_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

#include <iostream>
#include <list>
#include <vector>

namespace ben {	
	/*
		A Node is the vertex of a distributed directed graph structure. Each is managed by an Index, 
		but it is designed to be owned by any object the programmer wishes. The Nodes are connected by 
		Links, which are owned by the Node to which they are inputs. Links are not visible to the user; 
		they are accessed via input_port and output_port. These port objects are part bidirectional 
		iterator, part stream, and are used with operators only. An owning object treats the Node as a 
		message passer; handling the Links is abstracted away. 
		
		The owned (input) Links are stored in a linked list so as to avoid copying the Link objects.
		Because Nodes also store pointers to each output Link, copying a Link would mean updating this
		pointer. The output Link pointers are stored in a vector. Using vector helps the compiler cache-
		optimize access to a Link from the Link's origin. Any access to a Link from either the origin 
		or target Node should only require one layer of indirection, not counting the (hopefully)
		cache-optimized vector access. 
		
		Managing owned Nodes is done via public member functions, while managing output Node pointers is
		done with private members. These private members are primarily used by the Links to manage their 
		own pointers, so the Link ctors and dtor must be friended. 
		
		Node will be managed by a static Index member by default. It also has a static integer member to
		ensure that Node IDs are unique. 
		
		Each Node will have a mutex member when multithreading is implemented.
	*/
	
	template<typename W, typename S> 
	class Node {
	private:
		//make sure each Node has a unique ID
		static unsigned int IDCOUNT; 
		inline static unsigned int get_new_ID() { return IDCOUNT++; }
		
		std::list< Link<W,S> > inputs; //FIELD
		std::vector< Link<W,S>* > outputs; //FIELD, vector is better for the cache
		Index<W,S>* index; //FIELD
		
		friend class Index<W,S>; //managing Index needs to update its pointer
		bool update_index(Index<W,S>* const cIndex); //updates Link::index pointers too
		
		friend Link<W,S>::Link(	Index<W,S>* const pIndex,
		     			const unsigned int nOrigin, 
		     			const unsigned int nTarget, 
		     			const W& wWeight);
		friend Link<W,S>::Link(Link<W,S>&& rhs);
		friend Link<W,S>::~Link(); //Link ctor and dtor need to manage their pointers
		void update_output(const Link<W,S>* const oldLink, Link<W,S>* const newLink); //in case Link gets reallocated (const is broken here)
		void remove_output(Link<W,S>* const pLink); //only removes pointer
		void add_output(Link<W,S>* const pLink); //only adds pointer to vector
	
	public:
		Node& operator<<(const S& signal);
	
		static Index<W,S> INDEX;
		const unsigned int ID; //FIELD
		W bias; //FIELD
		
		//need to add constructor to set bias
		Node();
		explicit Node(const unsigned int nID); //managed by static Index by default
		explicit Node(const W& wBias, const unsigned int nID = get_new_ID());
		explicit Node(Index<W,S>& cIndex, const unsigned int nID = get_new_ID());
		Node(Index<W,S>& cIndex, const W& wBias, const unsigned int nID = get_new_ID());
		Node(const Node& rhs);
		Node(const Node& rhs, const unsigned int nID);
		Node(Node&& rhs); //move semantics to transfer all Links
		Node& operator=(const Node& rhs); //duplicates Node, including Links
		Node& operator=(Node&& rhs) = delete;
		~Node(); 
		
		const Index<W,S>& get_index() { return *index; }
		
		bool copy_inputs(const Node& other);
		bool copy_outputs(const Node& other);
		
		void add_input(	const unsigned int address,
				const W& weight); //creates Link object
		void remove_input(const unsigned int address); //destroys Link object
		void add_output(const unsigned int address,
				const W& weight);
		void remove_output(const unsigned int address);
		void clear(); //deletes all associated Links
		void clear_inputs();
		void clear_outputs();
		
		bool contains_input(const unsigned int nOrigin) const; 
		bool contains_output(const unsigned int nTarget) const;
	
		class input_port { //part bidirectional iterator, part stream object
		private:
			typename std::list< Link<W,S> >::iterator current;

			friend class Node;
			input_port(const typename std::list< Link<W,S> >::iterator iLink);
			
		public:
			input_port() = default;
			input_port(const input_port& rhs) = default;
			input_port& operator=(const input_port& rhs) = default;
			~input_port() = default;
			
			Link<W,S>& operator*() const { return *current; } //use this where possible
			Link<W,S>* operator->() const { return &*current; }
			
			input_port& operator++();
			input_port  operator++(int);
			input_port& operator--();
			input_port  operator--(int);
			
			bool operator==(const input_port& rhs) const //compare iterator locations
				{ return current==rhs.current; }
			bool operator!=(const input_port& rhs) const
				{ return current!=rhs.current; }
				
			input_port& operator>>(S& signal); //buffer access is only one way
		}; //class input_port
	
		class output_port { //see above input_port
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
			Link<W,S>* operator->() const { return *current; } //use this where possible
			
			output_port& operator++();
			output_port  operator++(int);
			output_port& operator--();
			output_port  operator--(int);
			
			bool operator==(const output_port& rhs) const
				{ return current==rhs.current; }
			bool operator!=(const output_port& rhs) const
				{ return current!=rhs.current; } 
				
			output_port& operator<<(const S& signal);
		}; //class output_port
		
		//similar to STL, but haven't added rbegin or rend yet
		input_port  input_begin()  { return input_port( inputs.begin() ); }
		input_port  input_end()    { return input_port( inputs.end() ); }
		output_port output_begin() { return output_port( outputs.begin() ); }
		output_port output_end()   { return output_port( outputs.end() ); }
		
	}; //class Node
	
	//initialize static members
	template<typename W, typename S>
	unsigned int Node<W,S>::IDCOUNT = 0;
	
	template<typename W, typename S> Index<W,S> Node<W,S>::INDEX; 
	//////////////////////////////
	
} //namespace ben

#endif
