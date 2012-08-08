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
	
	template<typename V, typename S> 
	struct BaseLink {
	public:
		typedef V value_type;
		typedef S signal_type;
	
		Signal<S> front;
		value_type value;
		
		BaseLink() = delete; //Links are meaningless without origin and target addresses
		
		BaseLink(const BaseLink& rhs) = delete; //no reason to have this
		BaseLink& operator=(const BaseLink& rhs) = delete; //would leave hanging pointers
		BaseLink& operator=(BaseLink&& rhs) = delete;
		~BaseLink() = default;
		
		bool ready() const { return front.ready; }
		
	protected:
		//following needs S to be default-constructible
		explicit BaseLink(const value_type& v) : value(v), front{false, S()} {}
	}; //class Link
	
	template<typename V, typename S, typename I=unsigned int>
	struct SyncLink : public BaseLink {
		typedef I id_type;
	
		Signal<signal_type> back;
		
		SyncLink(Index<SyncNode<V,S>>& const index, const id_type source, 
		     const id_type target, const V& v=V()) //needs V to be default-constructible
			: BaseLink(v), back{false, S()} {
			//should the link have to do this? it doesn't own the ports...
			//also, what if one of these adds fails? (especially the first)
			//index.find(source).add( InPort<self_type>(this, source) );
			//index.find(target).add( OutPort<self_type>(this, target) );
		}
		
		//need thread safety but preferably without locking
		//look up atomic operations 
		//	- these need to be built-in types 
		//	- use on Signal::ready?
		void push(const signal_type& signal) { 
			front = std::move( back );
			back = Signal<signal_type>{true, signal}; 
		}
		signal_type pull() {
			signal_type temp = std::move( front.data );
			//need to make this thread safe, is a simply copy (no move) enough?
			front = std::move( back ); 
			return temp;
		}
	}
	
	template<typename V, typename S, typename I=unsigned int>
	struct AsyncLink {
		typedef I id_type;
	
		AsyncLink(Index<AsyncNode<V,S>>& const index, const id_type source, 
		     const id_type target, const V& v)
			: value(v), front{false, S()} {
			//index.find(source).add( InPort<self_type>(this, source) );
			//index.find(target).add( OutPort<self_type>(this, target) );
		}
	
		//these methods require locking for thread safety
		void push(const signal_type& signal) { front = Signal<signal_type>{true, signal}; }
		signal_type pull() { 
			front.ready = false;
			return front.data;
		}
	}
	
} //namespace ben

#endif
