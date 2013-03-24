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

#include "python_utils.h"
#include "Graph.h"
#include "UndirectedNode.h"
#include "DirectedNode.h"
#include "Port.h"
#include "Path.h"

BOOST_PYTHON_MODULE(benpy) {
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

	class_< message_graph, boost::noncopyable, std::shared_ptr<message_graph> >("message_graph", no_init) 
		.def("create", &std::make_shared<message_graph>)
		.staticmethod("create")
		.def("__iter__", iterator<message_graph>()) 
		.def("manages", &message_graph::manages)
		.def("size", &message_graph::size);
		//.def("__getitem__", &associative_access<message_graph>::get, return_value_policy<reference_existing_object>); //could use a helper struct for safety checks

	def("merge", &merge<message_graph>);

	class_< input_port >("input_port", init<const input_port&>()) 
		.add_property("address", &input_port::get_address)
		.add_property("ready", &input_port::is_ready)
		.def("pull", &input_port::pull);
	
	class_< output_port >("output_port", init<const output_port&>()) 
		.add_property("address", &output_port::get_address)
		.add_property("ready", &output_port::is_ready)
		.def("pull", &output_port::push);

	class_< input_manager, boost::noncopyable >("input_manager", no_init)
		.def("__iter__", iterator<input_manager>())
		.def("__getitem__", &wrap_link_find<input_manager>::get, return_value_policy<return_by_value>())
		.def("contains", &input_manager::contains)
		.def("size", &input_manager::size);

	class_< output_manager, boost::noncopyable >("output_manager", no_init)
		.def("__iter__", iterator<output_manager>())
		.def("__getitem__", &wrap_link_find<output_manager>::get, return_value_policy<return_by_value>())
		.def("contains", &output_manager::contains)
		.def("size", &output_manager::size);

	class_< message_node, boost::noncopyable >("message_node", init<>()) //missing walk and find
		.def( init< id_type >() )
		.def( init< std::shared_ptr<message_graph> >() )
		.def( init< std::shared_ptr<message_graph>, id_type >() )
		.add_property("ID", &message_node::ID)
		.add_property("index", &message_node::get_index)
		.def("add_input", &wrap_input<message_node>::add)
		.def("remove_input", &wrap_input<message_node>::remove)
		.def("add_output", &wrap_output<message_node>::add)
		.def("remove_output", &wrap_output<message_node>::remove)
		.def("clear", &message_node::clear)
		.def("clear_inputs", &message_node::clear_inputs)
		.def("clear_outputs", &message_node::clear_outputs)
		.def("join_index", &message_node::join_index)
		.def("leave_index", &message_node::leave_index)
		.def_readonly("inputs", &message_node::inputs)
		.def_readonly("outputs", &message_node::outputs); 

	class_< undirected_graph, boost::noncopyable, std::shared_ptr<undirected_graph> >("undirected_graph", no_init) 
		.def("create", std::make_shared<undirected_graph>)
		.staticmethod("create")
		.def("__iter__", iterator<undirected_graph>()) 
		.def("manages", &undirected_graph::manages)
		.def("size", &undirected_graph::size);
		//.def("__getitem__", &associative_access<undirected_graph>::get) //could use a helper struct for safety checks

	def("merge", &merge<undirected_graph>);

	class_< path >("path", init<const path&>())
		.add_property("value", &path::get_value, &path::set_value)
		.add_property("address", &path::get_address);

	class_< undirected_node, boost::noncopyable >("undirected_node", init<>()) //missing walk and find
		.def( init< id_type >() )
		.def( init< std::shared_ptr<undirected_graph> >() )
		.def( init< std::shared_ptr<undirected_graph>, id_type >() )
		.add_property("ID", &undirected_node::ID)
		.add_property("index", &undirected_node::get_index)
		.def("add", &wrap_links<undirected_node>::add)
		.def("remove", &wrap_links<undirected_node>::remove)
		.def("clear", &undirected_node::clear)
		.def("__iter__", iterator<undirected_node>())
		.def("contains", &undirected_node::contains)
		.def("size", &undirected_node::size)
		.def("join_index", &undirected_node::join_index)
		.def("leave_index", &undirected_node::leave_index);

} //BOOST_PYTHON_MODULE

