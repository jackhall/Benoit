#ifndef BenoitLink_h
#define BenoitLink_h
//(c) Jack Hall 2011, licensed under GNU GPL v3

namespace ben {
	
	/*
		Links are the edges of a distributed directed graph structure. Each Link has an origin Node from which it conveys 
		data, and a target Node to which it conveys data. Links are owned by their target Node. A Link owns its own 
		pointer as it is stored in the origin Node. 
		
		The template parameters are shared with the owning Node. W is the type of the weight (for Links) or bias (for Nodes).
		S is the type of message passed between Nodes. The buffer is a statically allocated array of this type. The extra
		space (there are two) may be wasted, but static allocation allows for faster access than dynamic allocation. 
		
		The origin and target Nodes are stored by their ID rather than by pointer. This avoids hanging pointers if a Node 
		object gets moved around or copied. It does mean that the Link needs to have a copy of the Index pointer, though. 
		
		Links are not designed to be visible to the user of Benoit. They should be accessed via the Node::input_port or
		Node::output_port classes, which are a cross between iterators and stream objects. 
		
		Each Link will have a mutex member when multithreading is implemented. 
	*/
	template<typename W, typename S> 
	class Link {
	private: 
		S buffer[2]; //extra space in buffer is for feedforward time delays or indeterminate evaluation order
		bool mark[2]; //current buffer index: [read write], false -> 0 and true -> 1
		Index<W,S>* index; //copy of Node's index pointer, used to access origin Node pointer
		//also manages its pointer stored at the origin Node
	
	public:
		const unsigned int origin, target;
		W weight;
		
		Link() = delete; //Links are meaningless without origin and target addresses
		Link(Index<W,S>* const pIndex,
		     const unsigned int nOrigin, 
		     const unsigned int nTarget, 
		     const W& wWeight);
		Link(const Link& rhs); //for use by STL container (list for now, which does not copy elements)
		Link& operator=(const Link& rhs) = delete; //because origin and target are const
		~Link(); //cleans up pointer at origin Node
		
		void push(const S& data); //write to buffer
		S pull(); //read from buffer
		
		void update_index(Index<W,S>* const pIndex); //called when owning Node updates
	}; //class Link
} //namespace ben

#endif
