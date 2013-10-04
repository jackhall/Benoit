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
		Node() : base_type(), inputs(), outputs() {} 
		explicit Node(id_type id) : base_type(id), inputs(), outputs() {}
		explicit Node(std::shared_ptr<index_type> graph) 
            : base_type(graph), inputs(), outputs() {}
		Node(std::shared_ptr<index_type> graph, const id_type id) 
            : base_type(graph, id), inputs(), outputs() {}
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
            inputs.push_back(input_port_type(address, args...));
            //unlock this node here, or at the end? do I need a pair lock? probably
            //lock other node
            node_iter->outputs.push_back( output_port_type(inputs.back(), ID()) );
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
        template<typename FUNCTION>
        bool mirror(const self_type& other, FUNCTION clone) { //should other be guaranteed const?
			//a way to copy the pattern of links instead of the links themselves
			//links-to-self are preserved as such, links between this and other are untouched, as 
			//this would violate const-ness of other and the principle of least surprise
            if(!is_managed()) return false;
            auto graph_ptr = get_index();
            //new version of node-internal vector of input ports
            std::vector<input_port_type> new_inputs;
            new_inputs.reserve(other.size());
            for(const auto& port : other.inputs) {
                if( !graph_ptr->look_up(port.get_address()) ) return false;
                new_inputs.push_back(clone(port));
            }
            unlink_all();
            inputs = std::move(new_inputs);
            for(const auto& port : inputs) {
                auto node_ptr = graph_ptr->look_up(port.get_address());
                node_ptr->outputs.push_back( output_port_type(inputs.back(), ID()) );
            }
            return true;
		}
	}; //Node
	
	//typedefs to hide default Port and Buffer choices for message-passing graph
	template<typename S, typename IndexBase::id_type L = 1> 
	using MessageNode = Node< InPort< Buffer<S,L> >, OutPort< Buffer<S,L> > >;

	//default node for value graphs
	template<typename V>
	using DirectedNode = Node< Path<V>, Path<V> >;

} //namespace ben

#endif

