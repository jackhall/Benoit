#ifndef BenoitLink_h
#define BenoitLink_h

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011-2012  Jack Hall

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
	template<typename S>
	struct Signal {
		Signal(const Signal&& rhs) : ready(rhs.ready), 
					     data( std::move(rhs.data) ) {}
		Signal& operator=(const Signal&& rhs) {
			if(this != &rhs) {
				ready = rhs.ready;
				data = std::move( rhs.data );
			}
		}
		bool ready;
		S data;
	};
	
	template<typename V, typename S, bool B> 
	struct Link {
		typedef V value_type;
		typedef S signal_type;
	
		Signal<S> front;
		V value;
		
		Link() = delete; //Links are meaningless without origin and target addresses
		Link(Index<V,S>& const index, const unsigned int source, 
		     const unsigned int target, const V& v); 
		
		Link(const Link& rhs) = delete; //no reason to have this
		Link& operator=(const Link& rhs) = delete; //would leave hanging pointers
		Link& operator=(Link&& rhs) = delete;
		~Link() = default;
		
		bool ready() const { return front.ready; }
		void push(const S& signal);
		S pull();
	}; //class Link
	
	template<typename V, typename S>
	struct Link<true> {
		Signal<S> back;
		
		Link(Index<V,S>& const index, const unsigned int source, 
		     const unsigned int target, const V& v)
			: value(v), front{false, S()}, back{false, S()} {
			index.find(source).add_synch( InPort(this, source) );
			index.find(target).add_synch( OutPort(this, target) );
		}
		
		void push(const S& signal) { 
			front = std::move( back );
			back = Signal<S>{true, signal}; 
		}
		S pull() {
			S temp = std::move( front.data );
			front = std::move( back );
			return temp;
		}
	}
	
	template<typename V, typename S>
	struct Link<false> {
		Link(Index<V,S>& const index, const unsigned int source, 
		     const unsigned int target, const V& v)
			: value(v), front{false, S()} {
			index.find(source).add_asynch( InPort(this, source) );
			index.find(target).add_asynch( OutPort(this, target) );
		}
	
		void push(const S& signal) { front = Signal<S>{true, signal}; }
		S pull() { 
			front.ready = false;
			return front.data;
		}
	}
	
} //namespace ben

#endif
