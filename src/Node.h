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
#include <atomic>
#include <mutex>
#include "PullLink.h"
#include "Port.h"

namespace ben {	
	/*
		Overview needs major updates!
	
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
	
	template<typename I, typename O>
	class Node {
	public:
		static_assert(std::is_same<I::link_type, O::link_type>::value);
		static_assert(std::is_same<I::value_type, O::value_type>::value);
		static_assert(std::is_same<I::signal_type, O::signal_type>::value);
		static_assert(std::is_same<I::id_type, O::id_type>::value);
		
		typedef I input_port_type;
		typedef O output_port_type;
		typedef typename I::id_type	id_type;
		typedef typename I::value_type 	value_type;
		typedef typename I::signal_type	signal_type;
		typedef typename I::link_type	link_type; 
		typedef Index<Node> 		index_type;
		typedef typename std::vector<I>::iterator input_iterator;
		typedef typename std::vector<O>::iterator output_iterator;
	private:
		static std::atomic<id_type> IDCOUNT;  
		inline static id_type get_new_ID() 
			{ return IDCOUNT.fetch_add(1, std::memory_order_relaxed); }
	
		std::atomic<id_type> nodeID;
		std::vector<input_port_type> inputs; //maintain as heaps?
		std::vector<output_port_type> outputs;
		index_type* index; 
		std::mutex node_mutex;
		
		friend class Index<Node>;
		
	public:
		static index_type INDEX;
		
		explicit Node(const id_type nID=get_new_ID())
		explicit Node(index_type& cIndex, const id_type nID = get_new_ID());
		Node(const Node& rhs);
		Node(const Node& rhs, const id_type nID);
		Node(Node&& rhs); 
		Node& operator=(const Node& rhs); //duplicates Node, including Links but not ID
		Node& operator=(Node&& rhs);
		~Node(); 
		
		id_type ID() const { return nodeID.load(std::memory_order_consume); }
		index_type& get_index() { return *index; }
		
		void lock() { node_mutex.lock(); }
		bool try_lock() { return node_mutex.try_lock(); }
		void unlock() { node_mutex.unlock(); }
		
		input_iterator  find_input(const id_type address);
		output_iterator find_output(const id_type address);
		
		bool add_input(const id_type address, const value_type& value = V());
		bool add_output(const id_type address, const value_type& value = V());
		
		void remove_input(const input_iterator iter);
		void remove_input(const id_type address)
			{ remove_input( find_input(address) ); }
		void remove_output(const output_iterator iter);
		void remove_output(const id_type address);
			{ remove_output( find_output(address) ); }
		
		void clear_inputs(); //should these clean up other nodes?
		void clear_outputs();
		
		id_type size_inputs() const { return inputs.size(); }
		id_type size_outputs() const { return outputs.size(); }
		//other std::vector methods - assign, swap
		
		input_iterator  input_begin() 	{ return inputs.begin(); }
		input_iterator  input_end() 	{ return inputs.end(); }
		output_iterator output_begin() 	{ return outputs.begin(); }
		output_iterator output_end() 	{ return outputs.end(); }
		
	}; //Node

	//initialize static members
	template<typename I, typename O>
	Node<I,O>::id_type Node<I,O>::IDCOUNT = 100000;
	
	template<typename I, typename O>
	Index<Node<I,O>> Node<I,O>::INDEX;
	
	//typedef to hide default Port and Link choices
	template<typename V, typename S>
	using stdNode = Node< InPort< PullLink<V,S,2> >, OutPort< PullLink<V,S,2> > >;
	
	//methods - constructors
	template<typename I, typename O>
	Node<I,O>::Node(index_type& cIndex, const id_type nID) 
		: nodeID(nID), index(&cIndex) {
		if( !index->add(*this) ) throw;	//if index already has a node with that ID
	}
	
	template<typename I, typename O>
	Node<I,O>::Node(const id_type nID) : Node(Node::INDEX, nID) {}
	
	template<typename I, typename O>
	Node<I,O>::Node(const Node& rhs) : Node(rhs, get_new_ID()) {}
	
	template<typename I, typename O>
	Node<I,O>::Node(const Node& rhs, const id_type nID) 
		: Node(*rhs.index, nID) {
		//make new copies of links	
	}
	
	template<typename I, typename O>
	Node<I,O>::Node(Node&& rhs) 
		: nodeID(rhs.nodeID), index(rhs.index), //can't delgate; need Index::update, not Index::add
		  inputs( std::move(rhs.inputs) ), outputs( std::move(rhs.outputs) ) { 
		//call Index::update_node
	}
	
	//methods - assignment
	template<typename I, typename O>
	Node<I,O>&  Node<I,O>::operator=(const Node& rhs) { //duplicates Node, including Links but not ID
		if(this != &rhs) {
			//call Index::move_to
			nodeID = rhs.nodeID; //call before or after move_to?
			inputs = std::move( rhs.inputs );
			outputs = std::move( rhs.outputs );
		}
	}
	
	template<typename I, typename O>
	Node<I,O>&  Node<I,O>::operator=(Node&& rhs) {
		if(this != &rhs) {
			//call Index::move_to
			nodeID = rhs.nodeID;
			index = rhs.index;
			//make new copies of links
		}
	}
	
	//methods - destructor
	template<typename I, typename O>
	Node<I,O>::~Node() {
		//remove from index
		//clean up Links
		//should the node be locked during destruction? maybe use lock_guard?
	}
	
	template<typename I, typename O>
	input_iterator Node<I,O>::find_input(const id_type address) {}
	
	template<typename I, typename O>
	output_iterator Node<I,O>::find_output(const id_type address) {}
	
	template<typename I, typename O>
	bool Node<I,O>::add_input(const id_type address, const value_type& value = V()) {}
	
	template<typename I, typename O>
	bool Node<I,O>::add_output(const id_type address, const value_type& value = V()) {}
	
	template<typename I, typename O>
	void Node<I,O>::remove_input(const input_iterator iter) {}
	
	template<typename I, typename O>
	void Node<I,O>::remove_output(const output_iterator iter) {}
	
	template<typename I, typename O>
	void Node<I,O>::clear_inputs() {}
	
	template<typename I, typename O>
	void Node<I,O>::clear_outputs() {}
	
} //namespace ben

#endif

