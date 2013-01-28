#ifndef BenoitNode_h
#define BenoitNode_h

/*
    Benoit: a flexible framework for distributed graphs and spaces
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
#include "Singleton.h"
#include "Link.h"
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
	class Node : public Singleton { 
	public:
		typedef Graph<Node> 		index_type;
		typedef I 			input_port_type;
		typedef O 			output_port_type;
		typedef typename std::vector<I>::iterator input_iterator;
		typedef typename std::vector<O>::iterator output_iterator;
		typedef typename I::signal_type	signal_type;
		
	private:
		typedef Node 		self_type;
		typedef Singleton 	base_type;
	
		static_assert(std::is_same<id_type, typename I::id_type>::value, 
			      "Index and Port unique ID types don't match");
		static_assert(std::is_same<I, typename O::complement_type>::value,
			      "Port types should complement one another");
		static_assert(std::is_same<typename I::complement_type, O>::value,
			      "Port types should complement one another");
		static_assert(std::is_same<typename I::signal_type, typename O::signal_type>::value,
			      "Port signal types should match");
		static_assert(std::is_same<typename I::id_type, typename O::id_type>::value, 
			      "Ports using different unique ID types");
	
		std::vector<input_port_type> inputs; 
		std::vector<output_port_type> outputs;
		//std::mutex node_mutex;
		
		bool clone_links(const self_type& other);
		void clean_up_input();
		void clean_up_output();
		
	public:
		Node() = default;
		explicit Node(const id_type id) : base_type(id) {}
		explicit Node(index_type& graph) : base_type(graph) {}
		Node(index_type& graph, const id_type id) : base_type(graph, id) {}
		Node(const self_type& rhs) = delete;
		Node(self_type&& rhs); 
		Node& operator=(const self_type& rhs) = delete;
		Node& operator=(self_type&& rhs);
		~Node(); 
		
		//void lock() { node_mutex.lock(); }
		//bool try_lock() { return node_mutex.try_lock(); }
		//void unlock() { node_mutex.unlock(); }
		
		const index_type& get_index() const 
			{ return static_cast<const index_type&>(base_type::get_index()); }
		
		input_iterator  find_input(const id_type address);
		output_iterator find_output(const id_type address);
		
		bool add_input(const id_type address);
		bool add_output(const id_type address);
		
		void remove_input(const input_iterator iter);
		void remove_input(const id_type address)
			{ remove_input( find_input(address) ); }
		void remove_output(const output_iterator iter);
		void remove_output(const id_type address)
			{ remove_output( find_output(address) ); }
		
		void clear_inputs(); //should these clean up other nodes?
		void clear_outputs();
		void clear() { clear_inputs(); clear_outputs(); }
		
		size_t size_inputs() const { return inputs.size(); }
		size_t size_outputs() const { return outputs.size(); }
		
		bool contains_input(const id_type address) const { inputs.end() == find_input(address); }
		bool contains_output(const id_type address) const { outputs.end() == find_output(address); }
		//other std::vector methods - assign, swap
		
		input_iterator  ibegin() { return inputs.begin(); }
		input_iterator  iend() 	 { return inputs.end(); }
		output_iterator obegin() { return outputs.begin(); }
		output_iterator oend() 	 { return outputs.end(); }
		
	}; //Node
	
	//typedef to hide default Port and Link choices
	template<typename S>
	using stdNode = Node< InPort< Link<S,1> >, OutPort< Link<S,1> > >;
	
	//methods - constructors
	template<typename I, typename O>
	Node<I,O>::Node(Node&& rhs) : base_type( std::move(rhs) ), inputs( std::move(rhs.inputs) ), 
		  		      outputs( std::move(rhs.outputs) ) {}
	
	//methods - assignment
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
	bool Node<I,O>::clone_links(const self_type& other) {
		if(&get_index() == &other.get_index) {
			clear();
			id_type currentID;
			for(input_port_type& x : other.inputs) {
				currentID = x.source();
				if(currentID != ID()) add_input(currentID, x.get_value());
			}
		
			for(output_port_type& x : other.outputs) {
				currentID = x.source();
				if(currentID != ID()) add_output(currentID, x.get_value());
			}
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	void Node<I,O>::clean_up_input() 
		{ std::remove_if(inputs.begin(), inputs.end(), 
				 [](const input_port_type x){ return x.is_ghost(); }); }
	
	template<typename I, typename O>
	void Node<I,O>::clean_up_output() 
		{ std::remove_if(outputs.begin(), outputs.end(), 
				 [](const output_port_type x){ return x.is_ghost(); }); }
	
	template<typename I, typename O>
	typename Node<I,O>::input_iterator Node<I,O>::find_input(const id_type address) {
		auto it = ibegin();
		for(auto ite=iend(); it!=ite; ++it) 
			if(it->source() == address) break;

		return it;
	}
	
	template<typename I, typename O>
	typename Node<I,O>::output_iterator Node<I,O>::find_output(const id_type address) {
		auto it = obegin();
		for(auto ite=oend(); it!=ite; ++it) 
			if(it->target() == address) break;
		
		return it;
	}
	
	template<typename I, typename O>
	bool Node<I,O>::add_input(const id_type address) {
		if( get_index().contains(address) ) {
			if( contains_input(address) ) return false;
			inputs.push_back( input_port_type(address) );
			get_index().elem(address).outputs.push_back( output_port_type(inputs.back(), ID()) );
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	bool Node<I,O>::add_output(const id_type address) {
		if( get_index().contains(address) ) {
			if( contains_output(address) ) return false;
			outputs.push_back( output_port_type(address) );
			get_index().elem(address).inputs.push_back( input_port_type(outputs.back(), ID()) );
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	void Node<I,O>::remove_input(const input_iterator iter) {
		get_index().elem(iter->source()).clean_up_output(ID());
		inputs.erase(iter);
	}
	
	template<typename I, typename O>
	void Node<I,O>::remove_output(const output_iterator iter) {
		get_index().elem(iter->target()).clean_up_input(ID());
		outputs.erase(iter);
	}
	
	template<typename I, typename O>
	void Node<I,O>::clear_inputs() {
		for(input_port_type& x : inputs) get_index().elem(x.source()).clean_up_output();
		inputs.clear();
	}
	
	template<typename I, typename O>
	void Node<I,O>::clear_outputs() {
		for(output_port_type& x : outputs) get_index().elem(x.target()).clean_up_input();
		outputs.clear();
	}
	
} //namespace ben

#endif

