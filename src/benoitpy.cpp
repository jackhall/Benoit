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
	typedef stdMessageNode<signal_type> message_node;
	typedef message_node::input_type input_port;
	typedef message_node::output_type output_port;
	typedef Graph<message_node> message_graph;

	typedef int value_type;
	typedef stdUndirectedNode<value_type> undirected_node;
	typedef undirected_node::link_type path;
	typedef Graph<undirected_node> undirected_graph;

	typedef unsigned int id_type;

	class_< message_graph, boost::noncopyable >("message_graph", init<>()) 
		.def("__iter__", iterator<message_graph>()) 
		.def("contains", &message_graph::contains)
		.def("size", &message_graph::size)
		.def("elem", &message_graph::elem)
		.def("add", &message_graph::add)
		.def("remove", &message_graph::remove)
		.def("merge_into", &message_graph::merge_into)
		.def("clear", &message_graph::clear);
	
	class_< message_node >("message_node", init<>())
		.def( init<unsigned int>() )
		.def( init<weight_type>() )
		.def( init<weight_type, unsigned int>() )
		.def( init< message_graph& >() )
		.def( init< message_graph&, unsigned int >() )
		.def( init< message_graph&, weight_type >() )
		.def( init< message_graph&, weight_type, unsigned int >() )
		.def( init< message_node&, unsigned int >() )
		.def_readwrite("value", &message_node::bias)
		.add_property("ID", &message_node::ID)
		.def("get_index", &message_node::get_index, return_internal_reference<>())
		.def("copy_inputs", &message_node::copy_inputs)
		.def("copy_outputs", &message_node::copy_outputs)
		.def("add_input", &message_node::add_input)
		.def("remove_input", &message_node::remove_input)
		.def("add_output", addoutput)
		.def("remove_output", removeoutput)
		.def("clear", &message_node::clear)
		.def("clear_inputs", &message_node::clear_inputs)
		.def("clear_outputs", &message_node::clear_outputs)
		.def("contains_input", &message_node::contains_input)
		.def("contains_output", &message_node::contains_output)
		.def("size_inputs", &message_node::size_inputs)
		.def("size_outputs", &message_node::size_outputs)
		.def("input_begin", &message_node::input_begin)
		.def("input_end", &message_node::input_end)
		.def("output_begin", &message_node::output_begin)
		.def("output_end", &message_node::output_end);
	
	node_class.attr("INDEX") = message_node::INDEX;
	
	class_< message_node::input_port >("input_port")
		.def( self == self )
		.def( self != self )
		.def("pull", &message_node::input_port::pull)
		.def("next", &std_port<message_node::input_port>::next, return_internal_reference<>())
		.def("prev", &std_port<message_node::input_port>::prev, return_internal_reference<>())
		.add_property("ready", &std_port<message_node::input_port>::ready)
		.add_property("origin", &std_port<message_node::input_port>::get_origin)
		.add_property("target", &std_port<message_node::input_port>::get_target)
		.add_property("value", &std_port<message_node::input_port>::get_value,
				       &std_port<message_node::input_port>::set_value);
	
	class_< message_node::output_port >("output_port")
		.def( self == self )
		.def( self != self )
		.def("push", &message_node::output_port::push)
		.def("next", &std_port<message_node::output_port>::next, return_internal_reference<>())
		.def("prev", &std_port<message_node::output_port>::prev, return_internal_reference<>())
		.add_property("ready", &std_port<message_node::output_port>::ready)
		.add_property("origin", &std_port<message_node::output_port>::get_origin)
		.add_property("target", &std_port<message_node::output_port>::get_target)
		.add_property("value", &std_port<message_node::output_port>::get_value,
				       &std_port<message_node::output_port>::set_value);
	
}
