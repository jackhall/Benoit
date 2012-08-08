#ifndef BenoitAsyncNode_h
#define BenoitAsyncNode_h

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

#include <vector>

namespace ben {

	template<typename V, typename S>
	class AsyncNode {
	public:
		typedef unsigned int 	id_type;
		typedef V 		value_type;
		typedef S		signal_type;
		typedef Index<AsyncNode> 	index_type;
		typedef Link<V,S,false,id_type>	link_type; 
		typedef std::vector< InPort<link_type> >::iterator  input_iterator;
		typedef std::vector< OutPort<link_type> >::iterator output_iterator;
	private:		
		static id_type IDCOUNT; 
		inline static id_type get_new_ID() { return IDCOUNT++; }
	
		id_type nodeID;
		std::vector< InPort<link_type> > inputs;
		std::vector< OutPort<link_type> > outputs;
		index_type* index; 
		
	public:
		static index_type INDEX;
		
		explicit AsyncNode(const id_type nID=get_new_ID())
		explicit AsyncNode(index_type& cIndex, const id_type nID = get_new_ID());
		AsyncNode(const AsyncNode& rhs);
		AsyncNode(const AsyncNode& rhs, const id_type nID);
		AsyncNode(AsyncNode&& rhs); 
		AsyncNode& operator=(const AsyncNode& rhs); //duplicates Node, including Links
		AsyncNode& operator=(AsyncNode&& rhs);
		~Node(); 
		
		input_iterator  find_input(const id_type address);
		output_iterator find_output(const id_type address);
		
		void add_input(const id_type address, const value_type& value = V());
		void add_output(const id_type address, const value_type& value = V());
		
		void remove_input(const id_type address);
		void remove_input(const input_iterator iter);
		void remove_output(const id_type address);
		void remove_output(const output_iterator iter);
		
		void clear_inputs(); //should these clean up other nodes?
		void clear_outputs();
		
		//other std::vector methods - assign, swap, size
		
		input_iterator  input_begin() 	{ return inputs.begin(); }
		input_iterator  input_end() 	{ return inputs.end(); }
		output_iterator output_begin() 	{ return outputs.begin(); }
		output_iterator output_end() 	{ return outputs.end(); }
		
	}; //AsyncNode

	//initialize static members
	template<typename V, typename S>
	unsigned int AsyncNode<V,S>::IDCOUNT = 100000;
	
	template<typename V, typename S> Index<AsyncNode<V,S>> AsyncNode<V,S>::INDEX;
	
	template<typename V, typename S>
	AsyncNode<V,S>::AsyncNode(const id_type nID) : AsyncNode(INDEX, nID) {}
	
	template<typename V, typename S>
	AsyncNode<V,S>::AsyncNode(index_type& cIndex, const id_type nID) 
		: nodeID(nID), index(&cIndex) {
		index->add(*this);	
	}
	
} //namespace ben
