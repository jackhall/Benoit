/*
	Eiffel: the simulation of a self-organizing network of trusses
	Copyright (C) 2012  John Wendell Hall

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
	
	The author may be reached at jackhall@utexas.edu.
*/

#include <boost/python.hpp>
#include "Benoit.h"

template<class T>
struct std_port {
	typedef typename T::weight_type W;

	static T& next(T& x) {
		++x;
		return x;
	}

	static T& prev(T& x) {
		--x;
		return x;
	}
	
	static W get_value(const T& x) { return x->weight; }
	static void set_value(T& x, W& w) { x->weight = w; }
	static unsigned int get_origin(const T& x) { return x->origin; }
	static unsigned int get_target(const T& x) { return x->target; }
	static bool ready(const T& x) { return x->ready(); }
	//need: pop, update_index, values
};

BOOST_PYTHON_MODULE(benoitpy) {
	using namespace boost::python;
	using namespace ben;
	
	typedef double 	signal_type;
	typedef int 	weight_type;
	
	class_< Index<weight_type, signal_type>, boost::noncopyable >("index", init<>()) 
		.def("__iter__", iterator<Index<weight_type, signal_type>>()) //need to verify Index::iterator traits
		.def("contains", &Index<weight_type, signal_type>::contains)
		.def("add", &Index<weight_type, signal_type>::add)
		.def("remove", &Index<weight_type, signal_type>::remove)
		.def("update_node", &Index<weight_type, signal_type>::update_node)
		.def("move_to", &Index<weight_type, signal_type>::move_to)
		.def("swap_with", &Index<weight_type, signal_type>::swap_with)
		.def("merge_into", &Index<weight_type, signal_type>::merge_into);
	
	//these function pointers aren't working... need way to resolve function overloads
	bool (Node<weight_type, signal_type>::*addoutput)(unsigned int, const weight_type&) = &Node<weight_type, signal_type>::add_output;
	void (Node<weight_type, signal_type>::*removeoutput)(unsigned int) = &Node<weight_type, signal_type>::remove_output;
	
	object node_class = 
	class_< Node<weight_type, signal_type> >("node", init<>())
		.def( init<unsigned int>() )
		.def( init<weight_type>() )
		.def( init<weight_type, unsigned int>() )
		.def( init< Index<weight_type, signal_type>& >() )
		.def( init< Index<weight_type, signal_type>&, unsigned int >() )
		.def( init< Index<weight_type, signal_type>&, weight_type >() )
		.def( init< Index<weight_type, signal_type>&, weight_type, unsigned int >() )
		.def( init< Node<weight_type, signal_type>&, unsigned int >() )
		.def_readwrite("value", &Node<weight_type, signal_type>::bias)
		.add_property("ID", &Node<weight_type, signal_type>::ID)
		.def("get_index", &Node<weight_type, signal_type>::get_index, return_internal_reference<>())
		.def("copy_inputs", &Node<weight_type, signal_type>::copy_inputs)
		.def("copy_outputs", &Node<weight_type, signal_type>::copy_outputs)
		.def("add_input", &Node<weight_type, signal_type>::add_input)
		.def("remove_input", &Node<weight_type, signal_type>::remove_input)
		.def("add_output", addoutput)
		.def("remove_output", removeoutput)
		.def("clear", &Node<weight_type, signal_type>::clear)
		.def("clear_inputs", &Node<weight_type, signal_type>::clear_inputs)
		.def("clear_outputs", &Node<weight_type, signal_type>::clear_outputs)
		.def("contains_input", &Node<weight_type, signal_type>::contains_input)
		.def("contains_output", &Node<weight_type, signal_type>::contains_output)
		.def("size_inputs", &Node<weight_type, signal_type>::size_inputs)
		.def("size_outputs", &Node<weight_type, signal_type>::size_outputs)
		.def("input_begin", &Node<weight_type, signal_type>::input_begin)
		.def("input_end", &Node<weight_type, signal_type>::input_end)
		.def("output_begin", &Node<weight_type, signal_type>::output_begin)
		.def("output_end", &Node<weight_type, signal_type>::output_end);
	
	node_class.attr("INDEX") = Node<weight_type, signal_type>::INDEX;
	
	class_< Node<weight_type, signal_type>::input_port >("input_port")
		.def( self == self )
		.def( self != self )
		.def("pull", &Node<weight_type, signal_type>::input_port::pull)
		.def("next", &std_port<Node<weight_type, signal_type>::input_port>::next, return_internal_reference<>())
		.def("prev", &std_port<Node<weight_type, signal_type>::input_port>::prev, return_internal_reference<>())
		.add_property("ready", &std_port<Node<weight_type, signal_type>::input_port>::ready)
		.add_property("origin", &std_port<Node<weight_type, signal_type>::input_port>::get_origin)
		.add_property("target", &std_port<Node<weight_type, signal_type>::input_port>::get_target)
		.add_property("value", &std_port<Node<weight_type, signal_type>::input_port>::get_value,
				       &std_port<Node<weight_type, signal_type>::input_port>::set_value);
	
	class_< Node<weight_type, signal_type>::output_port >("output_port")
		.def( self == self )
		.def( self != self )
		.def("push", &Node<weight_type, signal_type>::output_port::push)
		.def("next", &std_port<Node<weight_type, signal_type>::output_port>::next, return_internal_reference<>())
		.def("prev", &std_port<Node<weight_type, signal_type>::output_port>::prev, return_internal_reference<>())
		.add_property("ready", &std_port<Node<weight_type, signal_type>::output_port>::ready)
		.add_property("origin", &std_port<Node<weight_type, signal_type>::output_port>::get_origin)
		.add_property("target", &std_port<Node<weight_type, signal_type>::output_port>::get_target)
		.add_property("value", &std_port<Node<weight_type, signal_type>::output_port>::get_value,
				       &std_port<Node<weight_type, signal_type>::output_port>::set_value);
	
}
