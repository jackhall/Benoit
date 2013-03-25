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

	typedef int value_type;
	typedef stdUndirectedNode<value_type> undirected_node;
	typedef undirected_node::link_type path;

	typedef unsigned int id_type;

	BENOIT_TRANSLATE_INPUTPORT(input_port)
	BENOIT_TRANSLATE_OUTPUTPORT(output_port)

	BENOIT_TRANSLATE_DIRECTEDNODE(message_node, message_graph)

//	class_< undirected_graph, boost::noncopyable, std::shared_ptr<undirected_graph> >("undirected_graph", no_init) 
//		.def("create", std::make_shared<undirected_graph>)
//		.staticmethod("create")
//		.def("__iter__", iterator<undirected_graph>()) 
//		.def("manages", &undirected_graph::manages)
//		.def("size", &undirected_graph::size);
		//.def("__getitem__", &associative_access<undirected_graph>::get) //could use a helper struct for safety checks

	BENOIT_TRANSLATE_PATH(path) //no semicolon!

	BENOIT_TRANSLATE_UNDIRECTEDNODE(undirected_node, undirected_graph)

} //BOOST_PYTHON_MODULE

