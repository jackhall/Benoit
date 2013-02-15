#ifndef BenoitDirectedNode_h
#define BenoitDirectedNode_h

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
#include "LinkManager.h"

namespace ben {	
/* A DirectedNode is the vertex of a distributed directed graph structure. Each is managed by an Index, 
 * but it is designed to be owned by any object the programmer wishes. The DirectedNodes are connected by 
 * Buffers, which are abstracted away through Ports. These port objects are accessed by bidirectional 
 * iterators. Would there be problems treating them as streams? An owning object treats the DirectedNode as a 
 * message passer; handling the Buffers is abstracted away from the user as well.
 *
 * Ports are stored in vectors for good cache optimization; copying them is designed to be cheap. There
 * are two layers of indirection between a DirectedNode and its links: one to access ports in their vector, and
 * one to dereference the shared_ptr in each link. The first is necessary because the number of links
 * can't be known at compile-time, and the second because if links are actually stored in a DirectedNode, 
 * thread safety is impossible. 
 */
	template<typename I, typename O>
	class DirectedNode : public Singleton { 
	public:
		typedef Graph<DirectedNode> 	index_type;
		typedef I 			input_type;
		typedef O 			output_type;
		typedef typename LinkManager<input_type>::iterator input_iterator;
		typedef typename LinkManager<output_type>::iterator output_iterator;
		
	private:
		typedef DirectedNode 	self_type;
		typedef Singleton 	base_type;
	
		//Benoit would probably not compile anyway, but these assertions should
		//make debugging easier
		static_assert(std::is_same<id_type, typename I::id_type>::value, 
			      "Index and Port unique ID types don't match");
		static_assert(std::is_same<I, typename O::complement_type>::value,
			      "Port types should complement one another");
		static_assert(std::is_same<typename I::complement_type, O>::value,
			      "Port types should complement one another");
		static_assert(std::is_same<typename I::id_type, typename O::id_type>::value, 
			      "Ports using different unique ID types");
	
		LinkManager<input_type> inputs; 
		LinkManager<output_type> outputs;
		//std::mutex node_mutex;
		
		void clean_up_input(const id_type address) {
			//Since the cleanup methods are called by remove, delegating doesn't work.
			//The possibility of copying ports means that reference counting the links
			//does not necessarily work for identifying ghost links. Would there be a
			//performance hit for using weak_ptr in the copies? Is this safe?
			auto iter = find_input(address);
			if(iter != iend()) inputs.erase(iter);
		}
		void clean_up_output(const id_type address) {
			auto iter = find_output(address);
			if(iter != oend()) outputs.erase(iter);
		}
		
	public:
		DirectedNode() = default;
		explicit DirectedNode(const id_type id) : base_type(id) {}
		explicit DirectedNode(index_type& graph) : base_type(graph) {}
		DirectedNode(index_type& graph, const id_type id) : base_type(graph, id) {}
		DirectedNode(const self_type& rhs) = delete;
		DirectedNode& operator=(const self_type& rhs) = delete;
		DirectedNode(self_type&& rhs) : base_type(std::move(rhs)), inputs(std::move(rhs.inputs)),
					       outputs(std::move(rhs.outputs)) {}
		DirectedNode& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				inputs = std::move(rhs.inputs);
				outputs = std::move(rhs.outputs);
			}
		}
		virtual ~DirectedNode() { clear(); } //might want to lock while deleting links 
		
		//void lock() { node_mutex.lock(); }
		//bool try_lock() { return node_mutex.try_lock(); }
		//void unlock() { node_mutex.unlock(); }
		
		using base_type::ID;
		const index_type& get_index() const 
			{ return static_cast<const index_type&>(base_type::get_index()); }
		using base_type::is_managed;
		bool is_managed_by(const index_type& x) const { return base_type::is_managed_by(x); }

		input_iterator  find_input(const id_type address) { return inputs.find(address); }
		output_iterator find_output(const id_type address) { return outputs.find(address); }
	
		template<typename... Args>	
		bool add_input(const id_type address, Args... args) {
			if( get_index().contains(address) ) {
				if( contains_input(address) ) return false;
				inputs.add(get_index().elem(address).outputs, args...);
				return true;
			} else return false;
		}
		template<typename... Args>
		bool add_output(const id_type address, Args... args) {
			if( get_index().contains(address) ) {
				if( contains_output(address) ) return false;
				outputs.add(get_index().elem(address).inputs, args...);
				return true;
			} else return false;
		}
		bool clone_links(const self_type& other) {
			//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
			if( other.is_managed_by(get_index()) ) {
				clear();
				id_type currentID;
				LinkManager<input_type> target;
				LinkManager<output_type> source;

				for(const auto& x : other.inputs) {
					currentID = x.get_address();
					if(currentID != ID()) {
						source = get_index().elem(currentID).outputs;
						inputs.add_clone_of(x, source);
					}
				}
		
				for(const auto& x : other.outputs) {
					currentID = x.get_address();
					if(currentID != ID()) {
						target = get_index().elem(currentID).inputs;
						outputs.add_clone_of(x, target);
					}
				}

				return true;
			} else return false;
		}

		void remove_input(const input_iterator iter) {
			auto address = iter->get_address();
			inputs.remove(walk(iter).outputs, iter);
		}
		void remove_input(const id_type address) {
			inputs.remove(get_index().elem(address).outputs);
		}
		void remove_output(const output_iterator iter) {
			auto address = iter->target();
			outputs.remove(walk(iter).inputs, iter);
		}
		void remove_output(const id_type address) {
			outputs.remove(get_index().elem(address).inputs);
		}
		
		void clear_inputs() {
			for(auto iter=ibegin(); iter!=iend(); ++iter) walk(iter).outputs.clean_up(ID());
			inputs.clear();
		}
		void clear_outputs() {
			for(auto iter=obegin(); iter!=oend(); ++iter) walk(iter).inputs.clean_up(ID());
			outputs.clear();
		}
		void clear() { clear_inputs(); clear_outputs(); }
		
		size_t size_inputs() const { return inputs.size(); }
		size_t size_outputs() const { return outputs.size(); }
		
		bool contains_input(const id_type address) const 
			{ return inputs.end() != const_cast<self_type*>(this)->find_input(address); }
		bool contains_output(const id_type address) const 
			{ return outputs.end() != const_cast<self_type*>(this)->find_output(address); }
		//other std::vector methods - assign, swap

		self_type& walk(const input_iterator iter) { return get_index().elem(iter->get_address()); }//template this? const version?
		self_Type& walk(const output_iterator iter) { return get_index().elem(iter->get_address()); }
		input_iterator  ibegin() { return inputs.begin(); }
		input_iterator  iend() 	 { return inputs.end(); }
		output_iterator obegin() { return outputs.begin(); }
		output_iterator oend() 	 { return outputs.end(); }
		//is there a way to provide begin() and end() compatible with range-based for loops?
	}; //DirectedNode
	
	//typedefs to hide default Port and Buffer choices
	template<typename S, typename IndexBase::id_type L = 1> 
	using stdMessageNode = DirectedNode< InPort< Buffer<S,L> >, OutPort< Buffer<S,L> > >;

	template<typename V>
	using stdDirectedNode = DirectedNode< Path<V>, Path<V> >;

} //namespace ben

#endif

