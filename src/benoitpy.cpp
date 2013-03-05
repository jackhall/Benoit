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
#include "Graph.h"
#include "UndirectedNode.h"
#include "DirectedNode.h"
#include "Port.h"
#include "Path.h"

void IndexError() { PyErr_SetString(PyExc_IndexError, "No element with that ID"); }

template<class T>
struct associative_access { //helper class for element access
	typedef typename T::node_type V;
	typedef typename T::id_type id_type;

	static V& get(const T& x, id_type i) {
		auto iter = x.find(i);
		if(iter != x.end()) return *iter;
		else IndexError();
	}
};


BOOST_PYTHON_MODULE(benoitpy) {
	using namespace boost::python;
	using namespace ben;
	
	typedef double 	signal_type;
	typedef stdMessageNode<signal_type> message_node;
	typedef message_node::input_type input_port;
	typedef message_node::output_type output_port;
	typedef LinkManager<message_node, input_port> input_manager;
	typedef LinkManager<message_node, output_port> output_manager;
	typedef Graph<message_node> message_graph;
	typedef Index<message_node> message_index;

	typedef int value_type;
	typedef stdUndirectedNode<value_type> undirected_node;
	typedef undirected_node::link_type path;
	typedef Graph<undirected_node> undirected_graph;

	typedef unsigned int id_type;

	class_< message_graph, boost::noncopyable >("message_graph", init<>()) 
		.def("__iter__", iterator<message_graph>()) 
		.def("contains", &message_graph::contains)
		.def("size", &message_graph::size)
		//.def("__getitem__", &associative_access<message_graph>::get) //could use a helper struct for safety checks
		.def("add", &message_graph::add)
		.def("remove", &message_graph::remove)
		.def("merge_into", &message_graph::merge_into)
		.def("clear", &message_graph::clear);
	
	class_< message_node, boost::noncopyable >("message_node", init<>()) //missing walk and find
		.def( init< id_type >() )
		.def( init< message_graph& >() )
		.def( init< message_graph&, id_type >() )
		.add_property("ID", &message_node::ID)
		.add_property("index", &message_node::get_index)
		.def("add_input", &message_node::add_input)
		.def("remove_input", &message_node::remove_input)
		.def("add_output", &message_node::add_output)
		.def("remove_output", &message_node::remove_output)
		.def("clear", &message_node::clear)
		.def("clear_inputs", &message_node::clear_inputs)
		.def("clear_outputs", &message_node::clear_outputs);
		//.def_readonly("inputs", &message_node::inputs)
		//.def_readonly("outputs", &message_node::outputs);
/*
	class_< input_manager, boost::noncopyable >("input_manager")
		.def("__iter__", iterator<input_manager>())
		.def("contains", &input_manager::contains)
		.def("size", &input_manager::size); //need a helper struct to give associative access

	class_< output_manager, boost::noncopyable >("output_manager")
		.def("__iter__", iterator<output_manager>())
		.def("contains", &output_manager::contains)
		.def("size", &output_manager::size); //need a helper struct to give associative access

	class_< input_port >("input_port")
		.def( self == self )
		.def( self != self )
		.def("pull", &input_port::pull)
		.add_property("ready", &input_port::is_ready)
		.add_property("address", &input_port::get_address);
	
	class_< output_port >("output_port")
		.def( self == self )
		.def( self != self )
		.def("push", &output_port::push)
		.add_property("ready", &output_port::is_ready)
		.add_property("address", &output_port::get_address);
	
	class_< undirected_graph, boost::noncopyable >("undirected_graph", init<>()) 
		.def("__iter__", iterator<undirected_graph>()) 
		.def("contains", &undirected_graph::contains)
		.def("size", &undirected_graph::size)
		.def("__getitem__", &associative_access<undirected_graph>::get) //could use a helper struct for safety checks
		.def("add", &undirected_graph::add)
		.def("remove", &undirected_graph::remove)
		.def("merge_into", &undirected_graph::merge_into)
		.def("clear", &undirected_graph::clear);

	class_< undirected_node, boost::noncopyable >("undirected_node", init<>()) //missing walk and find
		.def( init< id_type >() )
		.def( init< undirected_graph& >() )
		.def( init< undirected_graph&, id_type >() )
		.add_property("ID", &undirected_node::ID)
		.add_property("index", &undirected_node::get_index)
		.def("add", &undirected_node::add)
		.def("remove", &undirected_node::remove)
		.def("clear", &undirected_node::clear)
		.def("__iter__", iterator<undirected_node>())
		.def("contains", &undirected_node::contains)
		.def("size", &undirected_node::size);

	class_< path >("path")
		.def( self==self )
		.def( self!=self )
		.add_property("value", &path::get_value, &path::set_value)
		.add_property("address", &path::get_address);
*/
}
