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

#include <vector>
#include <mutex>
#include <algorithm>
#include "Singleton.h"
#include "Path.h"
#include "Port.h"

namespace ben {	
/* A Node is the vertex of a distributed directed graph structure. Each is managed by an Index, 
 * but it is designed to be owned by any object the programmer wishes. The Nodes are connected by 
 * Buffers, which are abstracted away through Ports. These port objects are accessed by bidirectional 
 * iterators. Would there be problems treating them as streams? An owning object treats the Node as a 
 * message passer; handling the Buffers is abstracted away from the user as well.
 *
 * Ports are stored in vectors (inside LinkManagers) for good cache optimization; copying them is designed to be cheap. There
 * are two layers of indirection between a Node and its links: one to access ports in their vector, and
 * one to dereference the shared_ptr in each link. The first is necessary because the number of links
 * can't be known at compile-time, and the second because if links are actually stored in a Node, 
 * thread safety is impossible (simultaneously moving two connected nodes at one time would give undefined behavior).  
 */
	
    //should inherit from port_traits<IN,OUT>
    template<typename IN, typename OUT>
	class Node : public Singleton { 
		typedef Node 	self_type;
		typedef Singleton 	base_type;

	public:
		typedef Graph<Node> 	index_type;
		typedef IN 			    input_port_type;
		typedef OUT 		    output_port_type;
		
	private:
		std::vector<input_port_type> inputs; 
		std::vector<output_port_type> outputs;
		typedef typename vector<input_port_type>::iterator input_iterator;
		//std::mutex node_mutex; //would need this to alter graph structure in multiple threads
        
		void perform_leave() { unlink_all(); }
	    input_iterator find_input(id_type address) const {
            for(input_iterator it = inputs.begin(); it!=inputs.end(); ++it) 
                if(it->get_address() == address) break;
            return it;
        }
        void remove_hook(id_type address) {
            std::remove_if(outputs.begin(), outputs.end(), [address](const output_port_type& port) {
                return port.get_address() == address; 
            });
        }
        
	public:
		//For the ctors lacking an id_type argument, Singleton automatically generates a unique ID.
		//This generated ID is only guaranteed to be unique if that generation method is used exclusively.
		Node() : base_type(), inputs(ID()), outputs(ID()) {} 
		explicit Node(id_type id) : base_type(id), inputs(id), outputs(id) {}
		explicit Node(std::shared_ptr<index_type> graph) 
            : base_type(graph), inputs(ID()), outputs(ID()) {}
		Node(std::shared_ptr<index_type> graph, const id_type id) 
            : base_type(graph, id), inputs(id), outputs(id) {}
		Node(const self_type& rhs) = delete; //identity semantics
		Node& operator=(const self_type& rhs) = delete;
		Node(self_type&& rhs) 
			: base_type(std::move(rhs)), 
			  inputs(std::move(rhs.inputs)),
		      outputs(std::move(rhs.outputs)) {}
		Node& operator=(self_type&& rhs) {
			if(this != &rhs) {
				base_type::operator=( std::move(rhs) );
				inputs = std::move(rhs.inputs);
				outputs = std::move(rhs.outputs);
			}
		}
		virtual ~Node() { 
            if(!is_managed()) return;
            //need to lock this node
            unlink_all(); 
            for(auto iter=outputs.begin(); iter != outputs.end(); ++iter) 
                get_index()->look_up(iter->get_address())->unlink(ID()); //need to lock other node
        } //might want to lock while deleting links 
	
		bool join_index(std::shared_ptr<index_type> ptr) { return base_type::join_index(ptr); }
		std::shared_ptr<index_type> get_index() const { 
            return std::static_pointer_cast<index_type>(base_type::get_index()); 
        }

        template<class FUNCTION>
        FUNCTION for_inputs(FUNCTION f) const {
            //lock this node
            for(auto iter=inputs.begin(); iter!=inputs.end(); ++iter) 
                f(*iter);
            return f;
            //unlock
        }
        template<class FUNCTION>
        FUNCTION for_outputs(FUNCTION f) const {
            //lock this node
            for(auto iter=outputs.begin(); iter!=outputs.end(); ++iter) 
                f(*iter);
            return f;
            //unlock
        }

		template<typename... ARGS>	
		bool link(id_type address, ARGS... args) {
            //lock this node
            if(!is_managed()) return false;
			auto node_ptr = get_index()->look_up(address);
            //is there a node with this address?
			if(!node_ptr) return false;
            //is there already a link to this other Node?
            if(linked_to(address)) return false;
            input_port_type new_port(address, args...);
            inputs.push_back(new_port);
            //unlock this node here, or at the end? do I need a pair lock? probably
            //lock other node
            node_iter->outputs.push_back( output_port_type(new_port, ID()) );
            //unlock other node
            return true;
		}
		void unlink(id_type address) {
			//O(n), must search for the right port
            //lock this node
            auto port_iter = find_input(address);
            if(port_iter == inputs.end()) return; //do nothing if the link does not exist
            //checking for address validity is unnecessary if the link exists
			inputs.erase(port_iter);
            //unlock this node here, or at the end? should match pattern in add
            get_index()->look_up(address)->remove_hook(ID());
		}
		void unlink_all() { 
			//cleaning up after all links before deleting them prevents iterator invalidation
			for(auto iter=inputs.begin(); iter!=inputs.end(); ++iter) 
                get_index()->look_up(iter->get_address())->remove_hook(ID());
			inputs.clear();
		}
        bool linked_to(id_type address) { return inputs.end() != find_input(address); }
        size_t size() const { return inputs.size(); }
        //pass in a function that clones links?
        //bool mirror(const self_type& other) { //should other be guaranteed const?
		//	//a way to copy the pattern of links instead of the links themselves
		//	//links-to-self are preserved as such, links between this and other are untouched, as 
		//	//this would violate const-ness of other and the principle of least surprise
		//	if( get_index() == other.get_index() ) {
		//		//the code in each of these lambdas would have to be written twice
		//		auto clear_inputs_except = [this](const id_type id, const index_type& index) {
		//			for(auto iter=inputs.begin(); iter!=inputs.end(); ++iter) 
		//				if(iter->get_address() != id) 
		//					index.elem(iter->get_address()).outputs.clean_up(ID());
		//			inputs.clear();
		//		};

		//		auto clear_outputs_except = [this](const id_type id, const index_type& index) {
		//			for(auto iter=outputs.begin(); iter!=outputs.end(); ++iter) 
		//				if(iter->get_address() != id) 
		//					index.elem(iter->get_address()).inputs.clean_up(ID());
		//			outputs.clear();
		//		};

		//		//take care of links between this and other, which would ordinarily
		//		//be deleted before the mirror operation
		//		auto input_iter = inputs.find(ID());
		//		auto output_iter = outputs.find(ID());
		//		if(input_iter != inputs.end() and output_iter != outputs.end()) {
		//			//this case saves both links
		//			auto input_temp = *input_iter;
		//			auto output_temp = *output_iter; 
		//			clear_inputs_except(other.ID(), *get_index());
		//			clear_outputs_except(other.ID(), *get_index());
		//			inputs.restore(input_temp, other.outputs); 
		//			outputs.restore(output_temp, other.inputs);
		//		} else if(input_iter != inputs.end() and output_iter == outputs.end()) { 
		//			//if other had an input from this, but not an output...
		//			auto input_temp = *input_iter; //save a copy of the Port
		//			clear_inputs_except(other.ID(), *get_index()); //deletes all Ports, but does not clean up after other
		//			inputs.restore(input_temp, other.outputs); //add the copy back
		//		} else if(input_iter == inputs.end() and output_iter != outputs.end()) { 
		//			//mirror of the last case
		//			auto output_temp = *output_iter;
		//			clear_outputs_except(other.ID(), *get_index());
		//			outputs.restore(output_temp, other.inputs);
		//		} else clear(); //no unusual problems

		//		//copy the rest of the inputs
		//		for(const auto& x : other.inputs) {
		//			id_type currentID = x.get_address();
		//			if(currentID != ID()) { //these links shouldn't be touched 
		//				if(currentID == other.ID()) inputs.add_clone_of(x, outputs); //only do this once 
		//				else {
		//					auto& source = get_index()->elem(currentID);
		//					inputs.add_clone_of(x, source.outputs);
		//				}
		//			}
		//		}

		//		//and the rest of the outputs
		//		for(const auto& x : other.outputs) {
		//			id_type currentID = x.get_address();
		//			if(currentID != ID() and currentID != other.ID()) { //links-to-self already copied
		//				auto& target = get_index()->elem(currentID);
		//				outputs.add_clone_of(x, target.inputs);
		//			}
		//		}

		//		return true;
		//	} else return false;
		//}
	}; //Node
	
	//typedefs to hide default Port and Buffer choices for message-passing graph
	template<typename S, typename IndexBase::id_type L = 1> 
	using MessageNode = Node< InPort< Buffer<S,L> >, OutPort< Buffer<S,L> > >;

	//default node for value graphs
	template<typename V>
	using DirectedNode = Node< Path<V>, Path<V> >;

} //namespace ben

#endif

