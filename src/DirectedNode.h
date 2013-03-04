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
#include "Path.h"
#include "Port.h"
#include "LinkManager.h"

namespace ben {	
/* A DirectedNode is the vertex of a distributed directed graph structure. Each is managed by an Index, 
 * but it is designed to be owned by any object the programmer wishes. The DirectedNodes are connected by 
 * Buffers, which are abstracted away through Ports. These port objects are accessed by bidirectional 
 * iterators. Would there be problems treating them as streams? An owning object treats the DirectedNode as a 
 * message passer; handling the Buffers is abstracted away from the user as well.
 *
 * Ports are stored in vectors (inside LinkManagers) for good cache optimization; copying them is designed to be cheap. There
 * are two layers of indirection between a DirectedNode and its links: one to access ports in their vector, and
 * one to dereference the shared_ptr in each link. The first is necessary because the number of links
 * can't be known at compile-time, and the second because if links are actually stored in a DirectedNode, 
 * thread safety is impossible (simultaneously moving two connected nodes at one time would give undefined behavior).  
 */
	template<typename I, typename O>
	class DirectedNode : public Singleton { 
	private:
		typedef DirectedNode 	self_type;
		typedef Singleton 	base_type;

	public:
		typedef Graph<DirectedNode> 	index_type;
		typedef I 			input_type;
		typedef O 			output_type;
		typedef typename LinkManager<self_type, input_type>::iterator input_iterator;
		typedef typename LinkManager<self_type, input_type>::const_iterator const_input_iterator;
		typedef typename LinkManager<self_type, output_type>::iterator output_iterator;
		typedef typename LinkManager<self_type, output_type>::const_iterator const_output_iterator;
		
	private:
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
	
		//std::mutex node_mutex; //would need this to alter graph structure in multiple threads
		
	public:
		LinkManager<self_type, input_type> inputs; //interface for LinkManagers is now restricted
		LinkManager<self_type, output_type> outputs;

		//For the ctors lacking an id_type argument, Singleton automatically generates a unique ID.
		//This generated ID is only guaranteed to be unique if that generation method is used exclusively.
		DirectedNode() : base_type(), inputs(ID()), outputs(ID()) {} 
		explicit DirectedNode(const id_type id) : base_type(id), inputs(id), outputs(id) {}
		explicit DirectedNode(index_type& graph) : base_type(graph), inputs(ID()), outputs(ID()) {}
		DirectedNode(index_type& graph, const id_type id) : base_type(graph, id), inputs(id), outputs(id) {}
		DirectedNode(const self_type& rhs) = delete; //identity semantics
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
	
		//locking should probably be internal when I do add it...	
		//void lock() { node_mutex.lock(); }
		//bool try_lock() { return node_mutex.try_lock(); }
		//void unlock() { node_mutex.unlock(); }
		
		using base_type::ID;
		const index_type& get_index() const 
			{ return static_cast<const index_type&>(base_type::get_index()); }
		using base_type::is_managed;
		bool is_managed_by(const index_type& x) const { return base_type::is_managed_by(x); }

		input_iterator find_input(const id_type address) { return inputs.find(address); }
		const_input_iterator find_input(const id_type address) const { return inputs.find(address); }
		output_iterator find_output(const id_type address) { return outputs.find(address); }
		const_output_iterator find_output(const id_type address) const { return outputs.find(address); }
	
		template<typename... ARGS>	
		bool add_input(const id_type address, ARGS... args) {
			//the type safety for this function comes in LinkManager::add
			if( get_index().contains(address) ) {
				return inputs.add(get_index().elem(address).outputs, args...);
			} else return false;
		}
		template<typename... ARGS>
		bool add_output(const id_type address, ARGS... args) {//see add_input
			if( get_index().contains(address) ) {
				return outputs.add(get_index().elem(address).inputs, args...);
			} else return false;
		}
		//cloning DirectedNodes is not well-defined for all cases without allowing redundant links 
		/*bool clone_links(const self_type& other) {
			//a way to explicitly copy a set of links, replaces the copy constructor for this purpose
			//how should links-to-self be cloned? need two versions of clone?
			if( other.is_managed_by(get_index()) ) {
				//these are not correct yet! do they have to be coupled?
				auto input_iter = find_input(other.ID());
				auto output_iter = find_output(other.ID());
				if(input_iter != iend() and output_iter != oend()) {
					auto input_temp = *input_iter;
					auto output_temp = *output_iter; //which should be used?
					clear();
					inputs.add_clone_of(input_temp, outputs); //these two lines result in two links-to-self!
					outputs.add_clone_of(output_temp, inputs);
				} else if(input_iter == iend() and output_iter != oend()) { //if other had an input from this, but not an output...
					auto output_temp = *output_iter;//save the endangered link
					clear();
					outputs.add_clone_of(output_temp, inputs);//add it back (it's now a link-to-self)
				} else if(input_iter != iend() and output_iter == oend()) { //mirror of the last case
					auto input_temp = *input_iter;
					clear();
					inputs.add_clone_of(input_temp, outputs);
				} else clear(); //no unusual problems

				//copy all inputs
				for(const auto& x : other.inputs) {
					id_type currentID = x.get_address();
					auto& source = get_index().elem(currentID);
					inputs.add_clone_of(x, source.outputs);
				}
				//and all outputs
				for(const auto& x : other.outputs) {
					currentID = x.get_address();
					auto& target = get_index().elem(currentID);
					outputs.add_clone_of(x, target.inputs);
				}

				return true;
			} else return false;
		}*/
		bool mirror(const self_type& other) { //should other be guaranteed const?
			//a way to copy the pattern of links instead of the links themselves
			//links-to-self are preserved as such, links between this and other are untouched, as 
			//this would violate const-ness of other and the principle of least surprise
			if( other.is_managed_by(get_index()) ) {
				//the code in each of these lambdas would have to be written twice
				auto clear_inputs_except = [this](const id_type id, const index_type& index) {
					for(auto iter=inputs.begin(); iter!=inputs.end(); ++iter) 
						if(iter->get_address() != id) 
							index.elem(iter->get_address()).outputs.clean_up(ID());
					inputs.clear();
				};

				auto clear_outputs_except = [this](const id_type id, const index_type& index) {
					for(auto iter=outputs.begin(); iter!=outputs.end(); ++iter) 
						if(iter->get_address() != id) 
							index.elem(iter->get_address()).inputs.clean_up(ID());
					outputs.clear();
				};

				//take care of links between this and other, which would ordinarily
				//be deleted before the mirror operation
				auto input_iter = find_input(ID());
				auto output_iter = find_output(ID());
				if(input_iter != iend() and output_iter != oend()) {
					//this case saves both links
					auto input_temp = *input_iter;
					auto output_temp = *output_iter; 
					clear_inputs_except(other.ID(), get_index());
					clear_outputs_except(other.ID(), get_index());
					inputs.restore(input_temp, other.outputs); 
					outputs.restore(output_temp, other.inputs);
				} else if(input_iter != iend() and output_iter == oend()) { 
					//if other had an input from this, but not an output...
					auto input_temp = *input_iter; //save a copy of the Port
					clear_inputs_except(other.ID(), get_index()); //deletes all Ports, but does not clean up after other
					inputs.restore(input_temp, other.outputs); //add the copy back
				} else if(input_iter == iend() and output_iter != oend()) { 
					//mirror of the last case
					auto output_temp = *output_iter;
					clear_outputs_except(other.ID(), get_index());
					outputs.restore(output_temp, other.inputs);
				} else clear(); //no unusual problems

				//copy the rest of the inputs
				for(const auto& x : other.inputs) {
					id_type currentID = x.get_address();
					if(currentID != ID()) { //these links shouldn't be touched 
						if(currentID == other.ID()) inputs.add_clone_of(x, outputs); //only do this once 
						else {
							auto& source = get_index().elem(currentID);
							inputs.add_clone_of(x, source.outputs);
						}
					}
				}

				//and the rest of the outputs
				for(const auto& x : other.outputs) {
					id_type currentID = x.get_address();
					if(currentID != ID() and currentID != other.ID()) { //links-to-self already copied
						auto& target = get_index().elem(currentID);
						outputs.add_clone_of(x, target.inputs);
					}
				}

				return true;
			} else return false;
		}

		void remove_input(const input_iterator iter) {
			//O(1); doesn't have to call LinkManager::find first
			auto address = iter->get_address();
			inputs.remove(walk(iter).outputs, iter);
		}
		void remove_input(const id_type address) {
			//O(n), must search for the right port
			inputs.remove(get_index().elem(address).outputs);
		}
		void remove_output(const output_iterator iter) { //see remove_input
			auto address = iter->get_address();
			outputs.remove(walk(iter).inputs, iter);
		}
		void remove_output(const id_type address) { //see remove_input
			outputs.remove(get_index().elem(address).inputs);
		}
		
		void clear_inputs() { 
			//cleaning up after all links before deleting them prevents iterator invalidation
			for(auto iter=ibegin(); iter!=iend(); ++iter) walk(iter).outputs.clean_up(ID());
			inputs.clear();
		}
		void clear_outputs() { //see clear_inputs
			for(auto iter=obegin(); iter!=oend(); ++iter) walk(iter).inputs.clean_up(ID());
			outputs.clear();
		}
		void clear() { clear_inputs(); clear_outputs(); }
		
		size_t size_inputs() const { return inputs.size(); }
		size_t size_outputs() const { return outputs.size(); }
		
		bool contains_input(const id_type address) const { return inputs.contains(address); }
		bool contains_output(const id_type address) const { return outputs.contains(address); }
		//other std::vector methods - assign, swap

		template<typename T>
		self_type& walk(const T iter) const { 
			//returns a reference to the node that iter points to: walks the graph
			static_assert(	std::is_same<T, input_iterator>::value or
					std::is_same<T, const_input_iterator>::value or
					std::is_same<T, output_iterator>::value or
					std::is_same<T, const_output_iterator>::value,
					"cannot call walk without an iterator type");
			return get_index().elem(iter->get_address()); 
		}
		input_iterator  ibegin() { return inputs.begin(); }
		const_input_iterator ibegin() const { return inputs.begin(); }
		input_iterator  iend() 	 { return inputs.end(); }
		const_input_iterator iend() const { return inputs.end(); }
		output_iterator obegin() { return outputs.begin(); }
		const_output_iterator obegin() const { return outputs.begin(); }
		output_iterator oend() 	 { return outputs.end(); }
		const_output_iterator oend() const { return outputs.end(); }
		
		//is there a way to provide begin() and end() compatible with range-based for loops?
		//template<typename T>
		//struct IterRange { T::iterator begin(); T::iterator end(); };

	}; //DirectedNode
	
	//typedefs to hide default Port and Buffer choices for message-passing graph
	template<typename S, typename IndexBase::id_type L = 1> 
	using stdMessageNode = DirectedNode< InPort< Buffer<S,L> >, OutPort< Buffer<S,L> > >;

	//default node for value graphs
	template<typename V>
	using stdDirectedNode = DirectedNode< Path<V>, Path<V> >;

} //namespace ben

#endif

