/*
	Benoit: a library for distributed graphs
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
	//typedef stdMessageNode<signal_type> message_node;
	//typedef message_node::input_type input_port;
	//typedef message_node::output_type output_port;
	typedef Buffer<signal_type, 1> buffer;
	typedef InPort<buffer> input_port;
	typedef OutPort<buffer> output_port;

	typedef int value_type; //boost::python::object can't be used here because it's ctor isn't nothrow (might be fixed later)
	//typedef stdUndirectedNode<value_type> undirected_node;
	//typedef undirected_node::link_type path;
	typedef Path<value_type> path;

	BENOIT_EXPOSE_DIRECTEDGRAPH(input_port, output_port, message_node, message_graph)

//	class_< undirected_graph, boost::noncopyable, std::shared_ptr<undirected_graph> >("undirected_graph", no_init) 
//		.def("create", std::make_shared<undirected_graph>)
//		.staticmethod("create")
//		.def("__iter__", iterator<undirected_graph>()) 
//		.def("manages", &undirected_graph::manages)
//		.def("size", &undirected_graph::size);
		//.def("__getitem__", &associative_access<undirected_graph>::get) //could use a helper struct for safety checks

	BENOIT_EXPOSE_UNDIRECTEDGRAPH(path, undirected_node, undirected_graph)

} //BOOST_PYTHON_MODULE

