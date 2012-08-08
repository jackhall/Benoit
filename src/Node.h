#ifndef BenoitNode_h
#define BenoitNode_h

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

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
	
	template<typename V, typename S, bool B>
	class Node {
	public:
		typedef unsigned int 	id_type;
		typedef V 		value_type;
		typedef S		signal_type;
		typedef Index<Node> 	index_type;
		typedef Link<V,S,B,id_type>	link_type; 
		typedef std::vector< InPort<link_type> >::iterator  input_iterator;
		typedef std::vector< OutPort<link_type> >::iterator output_iterator;
	private:		
		static id_type IDCOUNT; 
		inline static id_type get_new_ID() { return IDCOUNT++; }
	
		id_type nodeID;
		std::vector< InPort<link_type> > inputs; //maintain as heaps?
		std::vector< OutPort<link_type> > outputs;
		index_type* index; 
		
	public:
		static index_type INDEX;
		
		explicit Node(const id_type nID=get_new_ID())
		explicit Node(index_type& cIndex, const id_type nID = get_new_ID());
		Node(const Node& rhs);
		Node(const Node& rhs, const id_type nID);
		Node(Node&& rhs); 
		Node& operator=(const Node& rhs); //duplicates Node, including Links
		Node& operator=(Node&& rhs);
		~Node(); 
		
		input_iterator  find_input(const id_type address);
		output_iterator find_output(const id_type address);
		
		void add_input(const id_type address, const value_type& value = V());
		void add_output(const id_type address, const value_type& value = V());
		
		void remove_input(const id_type address);
		void remove_input(const input_iterator iter);
		void remove_output(const id_type address);
		void remove_output(const output_iterator iter);
		
		void clear_inputs(); //should these clean up other nodes?
		void clear_outputs();
		
		//other std::vector methods - assign, swap, size
		
		input_iterator  input_begin() 	{ return inputs.begin(); }
		input_iterator  input_end() 	{ return inputs.end(); }
		output_iterator output_begin() 	{ return outputs.begin(); }
		output_iterator output_end() 	{ return outputs.end(); }
		
	}; //Node

	//initialize static members
	template<typename V, typename S, bool B>
	unsigned int Node<V,S,B>::IDCOUNT = 100000;
	
	template<typename V, typename S, bool B> Index<Node<V,S,B>> Node<V,S,B>::INDEX;
	
	//typedefs to eliminate third template parameter
	template<typename V, typename S>
	using SyncNode = Node<V,S,true>;
	
	template<typename V, typename S>
	using AsyncNode = Node<V,S,false>;
	
	//methods
	template<typename V, typename S, bool B>
	Node<V,S,B>::Node(const id_type nID) : Node(INDEX, nID) {}
	
	template<typename V, typename S, bool B>
	Node<V,S,B>::Node(index_type& cIndex, const id_type nID) 
		: nodeID(nID), index(&cIndex) {
		if( !index->add(*this) ) throw;	//index already has a node with that ID
	}
	
	template<typename V, typename S, bool B
	Node<V,S,B>::Node(const Node& rhs) : Node(rhs, get_new_ID()) {}
	
	template<typename V, typename S, bool B>
	Node<V,S,B>::Node(const Node& rhs, const id_type nID) 
		: Node(*rhs.index, nID) {
		//make new copies of links	
	}
	
} //namespace ben

#endif
