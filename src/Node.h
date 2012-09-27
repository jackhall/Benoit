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
	class Node : public Singleton<Graph<Node>> { //is this use of templates circular/illegal?
	public:
		static_assert(std::is_same<I::link_type, O::link_type>::value);
		static_assert(std::is_same<I::value_type, O::value_type>::value);
		static_assert(std::is_same<I::signal_type, O::signal_type>::value);
		static_assert(std::is_same<I::id_type, O::id_type>::value);
		
		typedef Node 			self_type;
		typedef Graph<Node> 		index_type;
		typedef Singleton<index_type> 	base_type;
		typedef I 			input_port_type;
		typedef O 			output_port_type;
		typedef typename I::id_type	id_type;
		typedef typename I::value_type 	value_type;
		typedef typename I::signal_type	signal_type;
		typedef typename I::link_type	link_type; 
		typedef typename std::vector<I>::iterator input_iterator;
		typedef typename std::vector<O>::iterator output_iterator;
		
	private:	
		std::vector<input_port_type> inputs; //maintain as heaps?
		std::vector<output_port_type> outputs;
		//std::mutex node_mutex;
		
		bool clone_links(const self_type& other);
		
	public:
		Node() = default;
		explicit Node(const id_type id) : base_type(id) {}
		explicit Node(index_type& graph) : base_type(graph) {}
		Node(index_type& graph, const id_type id) : base_type(graph, id) {}
		Node(const self_type& rhs); //copyable?
		Node(const self_type& rhs, const id_type id);
		Node(self_type&& rhs); 
		Node& operator=(const self_type& rhs); //duplicates Node, including Links but not ID
		Node& operator=(self_type&& rhs);
		~Node(); 
		
		//void lock() { node_mutex.lock(); }
		//bool try_lock() { return node_mutex.try_lock(); }
		//void unlock() { node_mutex.unlock(); }
		
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
		void clear() { clear_inputs(); clear_outputs(); }
		
		id_type size_inputs() const { return inputs.size(); }
		id_type size_outputs() const { return outputs.size(); }
		//other std::vector methods - assign, swap
		
		input_iterator  input_begin() 	{ return inputs.begin(); }
		input_iterator  input_end() 	{ return inputs.end(); }
		output_iterator output_begin() 	{ return outputs.begin(); }
		output_iterator output_end() 	{ return outputs.end(); }
		
	}; //Node
	
	//typedef to hide default Port and Link choices
	template<typename V, typename S>
	using stdNode = Node< InPort< PullLink<V,S,2> >, OutPort< PullLink<V,S,2> > >;
	
	//methods - constructors
	template<typename I, typename O>
	Node<I,O>::Node(const Node& rhs) : base_type(*rhs.index) {
		if( !clone_links(rhs) ) clear(); //should never happen
	}
	
	template<typename I, typename O>
	Node<I,O>::Node(const Node& rhs, const id_type nID) : base_type(*rhs.index, nID) {
		if( !clone_links(rhs) ) clear(); //should never happen
	}
	
	template<typename I, typename O>
	Node<I,O>::Node(Node&& rhs) 
		: nodeID(rhs.nodeID), index(rhs.index), //can't delgate; need Index::update, not Index::add
		  inputs( std::move(rhs.inputs) ), outputs( std::move(rhs.outputs) ) { 
		update_singleton(this);
	}
	
	//methods - assignment
	template<typename I, typename O>
	Node<I,O>&  Node<I,O>::operator=(const Node& rhs) { //duplicates Node, including Links but not ID
		if(this != &rhs) {
			clear();
			switch_index(rhs.index);
			if( !clone_links(rhs) ) clear(); //should never happen
		}
	}
	
	template<typename I, typename O>
	Node<I,O>&  Node<I,O>::operator=(Node&& rhs) {
		if(this != &rhs) {
			base_type::operator=( std::move(rhs) );
			inputs = std::move(rhs.inputs);
			outputs = std::move(rhs.outputs);
		}
	}
	
	//methods - destructor
	template<typename I, typename O>
	Node<I,O>::~Node() {} //might want to lock the node while deleting links
	
	template<typename I, typename O>
	void clone_links(const self_type& other) {
		id_type currentID;
		for(auto it=other.begin_inputs(), 
		    auto ite=other.end_inputs(); it!=ite; ++it) {
		    	currentID = it->source();
			if(currentID != uniqueID) add_input(currentID, it->get_value());
		}
		
		for(auto it=other.begin_outputs(), 
		    auto ite=other.end_outputs(); it!=ite; ++it) {
		    	currentID = it->target();
			if(currentID != uniqueID) add_input(currentID, it->get_value());
		}
	}
	
	template<typename I, typename O>
	input_iterator Node<I,O>::find_input(const id_type address) {
		auto it = input_begin();
		for(auto ite=input_end(); it!=ite; ++it) {
			if(it->source() == address) break;
		} 
		return it;
	}
	
	template<typename I, typename O>
	output_iterator Node<I,O>::find_output(const id_type address) {
		auto it = output_begin();
		for(auto ite=output_end(); it!=ite; ++it) {
			if(it->target() == address) break;
		} 
		return it;
	}
	
	template<typename I, typename O>
	bool Node<I,O>::add_input(const id_type address, const value_type& value = V()) {
		if( get_index().contains(address) ) {
			inputs.push_back( InPort(new link_type(value), address) );
			get_index().find(address).outputs.push_back( OutPort(inputs.back(), ID()) );
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	bool Node<I,O>::add_output(const id_type address, const value_type& value = V()) {
		if( get_index().contains(address) ) {
			outputs.push_back( OutPort(new link_type(value), address) );
			get_index().find(address).inputs.push_back( InPort(outputs.back(), ID()) );
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	void Node<I,O>::remove_input(const input_iterator iter) {
		//should this method clean up the other node?
	}
	
	template<typename I, typename O>
	void Node<I,O>::remove_output(const output_iterator iter) {}
	
	template<typename I, typename O>
	void Node<I,O>::clear_inputs() {
		for(auto it=inputs.begin(), 
		    auto ite=inputs.end(); it!=ite; ++it) {
			
		}
	}
	
	template<typename I, typename O>
	void Node<I,O>::clear_outputs() {
		for(auto it=outputs.begin(), 
		    auto ite=outputs.end(); it!=ite; ++it) {
			
		}
	}
	
} //namespace ben

#endif

