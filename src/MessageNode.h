#ifndef BenoitMessageNode_h
#define BenoitMessageNode_h

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
#include "Buffer.h"
#include "Port.h"

namespace ben {	
/* A MessageNode is the vertex of a distributed directed graph structure. Each is managed by an Index, 
 * but it is designed to be owned by any object the programmer wishes. The MessageNodes are connected by 
 * Buffers, which are abstracted away through Ports. These port objects are accessed by bidirectional 
 * iterators. Would there be problems treating them as streams? An owning object treats the MessageNode as a 
 * message passer; handling the Buffers is abstracted away from the user as well.
 *
 * Ports are stored in vectors for good cache optimization; copying them is designed to be cheap. There
 * are two layers of indirection between a MessageNode and its links: one to access ports in their vector, and
 * one to dereference the shared_ptr in each link. The first is necessary because the number of links
 * can't be known at compile-time, and the second because if links are actually stored in a MessageNode, 
 * thread safety is impossible. 
 */
	template<typename I, typename O>
	class MessageNode : public Singleton { 
	public:
		typedef Graph<MessageNode> 	index_type;
		typedef I 			input_type;
		typedef O 			output_type;
		typedef typename std::vector<I>::iterator input_iterator;
		typedef typename std::vector<O>::iterator output_iterator;
		typedef typename I::signal_type	signal_type;
		
	private:
		typedef MessageNode 	self_type;
		typedef Singleton 	base_type;
	
		//Benoit would probably not compile anyway, but these assertions should
		//make debugging easier
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
	
		std::vector<input_type> inputs; 
		std::vector<output_type> outputs;
		//std::mutex node_mutex;
		
		void clean_up_input(const id_type address);
		void clean_up_output(const id_type address);
		
	public:
		MessageNode() = default;
		explicit MessageNode(const id_type id) : base_type(id) {}
		explicit MessageNode(index_type& graph) : base_type(graph) {}
		MessageNode(index_type& graph, const id_type id) : base_type(graph, id) {}
		MessageNode(const self_type& rhs) = delete;
		MessageNode(self_type&& rhs); 
		MessageNode& operator=(const self_type& rhs) = delete;
		MessageNode& operator=(self_type&& rhs);
		virtual ~MessageNode(); 
		
		//void lock() { node_mutex.lock(); }
		//bool try_lock() { return node_mutex.try_lock(); }
		//void unlock() { node_mutex.unlock(); }
		
		using base_type::ID;
		const index_type& get_index() const 
			{ return static_cast<const index_type&>(base_type::get_index()); }
		using base_type::is_managed;
		bool is_managed_by(const index_type& x) const { return base_type::is_managed_by(x); }

		input_iterator  find_input(const id_type address);
		output_iterator find_output(const id_type address);
		
		bool add_input(const id_type address);
		bool add_output(const id_type address);
		bool clone_links(const self_type& other);

		void remove_input(const input_iterator iter);
		void remove_input(const id_type address);
		void remove_output(const output_iterator iter);
		void remove_output(const id_type address);
		
		void clear_inputs(); 
		void clear_outputs();
		void clear() { clear_inputs(); clear_outputs(); }
		
		size_t size_inputs() const { return inputs.size(); }
		size_t size_outputs() const { return outputs.size(); }
		
		bool contains_input(const id_type address) const 
			{ return inputs.end() != const_cast<self_type*>(this)->find_input(address); }
		bool contains_output(const id_type address) const 
			{ return outputs.end() != const_cast<self_type*>(this)->find_output(address); }
		//other std::vector methods - assign, swap
		
		input_iterator  ibegin() { return inputs.begin(); }
		input_iterator  iend() 	 { return inputs.end(); }
		output_iterator obegin() { return outputs.begin(); }
		output_iterator oend() 	 { return outputs.end(); }
		//is there a way to provide begin() and end() compatible with range-based for loops?
	}; //MessageNode
	
	//typedef to hide default Port and Buffer choices
	template<typename S>
	using stdMessageNode = MessageNode< InPort< Buffer<S,1> >, OutPort< Buffer<S,1> > >;
	
	//methods - constructors
	template<typename I, typename O>
	MessageNode<I,O>::MessageNode(MessageNode&& rhs) : base_type( std::move(rhs) ), inputs( std::move(rhs.inputs) ), 
		  		      outputs( std::move(rhs.outputs) ) {}
	
	//methods - assignment
	template<typename I, typename O>
	MessageNode<I,O>&  MessageNode<I,O>::operator=(MessageNode&& rhs) {
		if(this != &rhs) {
			base_type::operator=( std::move(rhs) );
			inputs = std::move(rhs.inputs);
			outputs = std::move(rhs.outputs);
		}
	}
	
	//methods - destructor
	template<typename I, typename O>
	MessageNode<I,O>::~MessageNode() { clear(); } //might want to lock the node while deleting links
	
	template<typename I, typename O>
	bool MessageNode<I,O>::clone_links(const self_type& other) {
	//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
		if(&(get_index()) == &(other.get_index())) {
			clear();
			id_type currentID;
			for(const input_type& x : other.inputs) {
				currentID = x.source();
				if(currentID != ID()) add_input(currentID);
			}
		
			for(const output_type& x : other.outputs) {
				currentID = x.target();
				if(currentID != ID()) add_output(currentID);
			}
			return true;
		} else return false;
	}

	template<typename I, typename O>	
	void MessageNode<I,O>::clean_up_input(const typename MessageNode<I,O>::id_type address) { 
		//Since the cleanup methods are called by remove, delegating doesn't work.
		//The possibility of copying ports means that reference counting the links
		//does not necessarily work for identifying ghost links. Would there be a
		//performance hit for using weak_ptr in the copies? Is this safe?
		auto iter = find_input(address);
		if(iter != iend()) inputs.erase(iter);
	}

	template<typename I, typename O>
	void MessageNode<I,O>::clean_up_output(const typename MessageNode<I,O>::id_type address) { 
		auto iter = find_output(address);
		if(iter != oend()) outputs.erase(iter);
	}

	template<typename I, typename O>
	typename MessageNode<I,O>::input_iterator MessageNode<I,O>::find_input(const id_type address) {
		auto it = ibegin();
		for(auto ite=iend(); it!=ite; ++it) 
			if(it->source() == address) break;

		return it;
	}
	
	template<typename I, typename O>
	typename MessageNode<I,O>::output_iterator MessageNode<I,O>::find_output(const id_type address) {
		auto it = obegin();
		for(auto ite=oend(); it!=ite; ++it) 
			if(it->target() == address) break;
		
		return it;
	}
	
	template<typename I, typename O>
	bool MessageNode<I,O>::add_input(const id_type address) {
		if( get_index().contains(address) ) {
			if( contains_input(address) ) return false;
			inputs.push_back( input_type(address) );
			get_index().elem(address).outputs.push_back( output_type(inputs.back(), ID()) );
			return true;
		} else return false;
	}
	
	template<typename I, typename O>
	bool MessageNode<I,O>::add_output(const id_type address) {
		if( get_index().contains(address) ) {
			if( contains_output(address) ) return false;
			outputs.push_back( output_type(address) );
			get_index().elem(address).inputs.push_back( input_type(outputs.back(), ID()) );
			return true;
		} else return false;
	}

	template<typename I, typename O>
	void MessageNode<I,O>::remove_input(const id_type address) {
		auto iter = find_input(address);
		if(iter != iend()) remove_input(iter);
	}
	
	template<typename I, typename O>
	void MessageNode<I,O>::remove_input(const input_iterator iter) {
		auto address = iter->source();
		inputs.erase(iter);
		get_index().elem(address).clean_up_output(ID());
	}

	template<typename I, typename O>
	void MessageNode<I,O>::remove_output(const id_type address) {
		auto iter = find_output(address);
		if(iter != oend()) remove_output(iter);
	}
	
	template<typename I, typename O>
	void MessageNode<I,O>::remove_output(const output_iterator iter) {
		auto address = iter->target();
		outputs.erase(iter);
		get_index().elem(address).clean_up_input(ID());
	}
	
	template<typename I, typename O>
	void MessageNode<I,O>::clear_inputs() {
		for(input_type& x : inputs) get_index().elem(x.source()).clean_up_output(ID());
		inputs.clear();
	}
	
	template<typename I, typename O>
	void MessageNode<I,O>::clear_outputs() {
		for(output_type& x : outputs) get_index().elem(x.target()).clean_up_input(ID());
		outputs.clear();
	}
	
} //namespace ben

#endif
