#ifndef python_utils_h
#define python_utils_h

#include <memory>
//because these definitions in the boost namespace need to be included before boost/python.hpp,
//this file should be included in lieu of boost/python.hpp
namespace boost {
	template<class T> 
	const T* get_pointer(const std::shared_ptr<T>& p) {
		return p.get();
	}

	template<class T> 
	T* get_pointer(std::shared_ptr<T>& p) {
		return p.get();
	}
} // namespace boost

#include <boost/python.hpp>
#include "LinkManager.h"

namespace ben {
	//////////////////////////////////
	//an error to throw for out-of-range element access
	void IndexError() { PyErr_SetString(PyExc_IndexError, "No element with that ID"); }

	//////////////////////////////
	//helper class for element access in LinkManagers
	template<typename T>
	struct wrap_link_find { 
		typedef typename T::link_type L;
		typedef typename T::id_type I;

		static L get(const T& x, I i) {
			auto iter = x.find(i);
			if(iter != x.end()) return *iter;
			else IndexError();
		}
	};

	///////////////////////////////
	//struct templates to help wrap DirectedNode::add_input while enforcing type safety
	template<typename N, typename... ARGS> 
	struct wrap_input_helper {};

	template<typename N, typename... ARGS>
	struct wrap_input_helper< N, ConstructionTypes<ARGS...> > {
		static bool add(N& x, typename N::id_type i, ARGS... args) {
			return x.add_input(i, args...);
		}

		static void remove(N& x, typename N::id_type i) {
			x.remove_input(i);
		}
	};

	template<typename N>
	using wrap_input = wrap_input_helper<N, typename N::input_type::construction_types>;


	//same for DirectedNode::add_output
	template<typename N, typename... ARGS> 
	struct wrap_output_helper {};

	template<typename N, typename... ARGS>
	struct wrap_output_helper< N, ConstructionTypes<ARGS...> > {
		static bool add(N& x, typename N::id_type i, ARGS... args) {
			return x.add_output(i, args...);
		}

		static void remove(N& x, typename N::id_type i) {
			x.remove_input(i);
		}
	};

	template<typename N>
	using wrap_output = wrap_output_helper<N, typename N::output_type::construction_types>;

	//bool (message_node::*add_message_input)(unsigned int) = &message_node::add_input;
	//bool (message_node::*add_message_output)(unsigned int) = &message_node::add_output;
	
	/////////////////////////////////
	//struct templates to help wrap UndirectedNode::add and remove
	template<typename N, typename... ARGS>
	struct wrap_link_helper {};

	template<typename N, typename... ARGS>
	struct wrap_link_helper< N, ConstructionTypes<ARGS...> > {
		static bool add(N& x, typename N::id_type i, ARGS... args) {
			return x.add(i, args...);
		}

		static void remove(N& x, typename N::id_type i) {
			x.remove(i);
		}
	};

	template<typename N>
	using wrap_links = wrap_link_helper<N, typename N::link_type::construction_types>;
	
} //namespace ben


//This macro expands a path type into a boost.python translation.
//Directions for this and the following macros:
//TO USE: pass in type names for the appropriate arguments
//NOTE: use type names that also work as type names in python!
//NOTE:these macros should all be used either inside the "ben" namespace or with 
//a "using namespace ben;" active
//NOTE:each use of each macro should be unique to avoid naming collisions
#define BENOIT_TRANSLATE_PATH(PATH) { \
class_< PATH >(#PATH, init<const PATH&>()) \
	.add_property("value", &PATH::get_value, &PATH::set_value) \
	.add_property("address", &path::get_address); }


//macro for UndirectedNode, missing walk and find, including a corresponding graph
//the graph is missing associative access to managed nodes
#define BENOIT_TRANSLATE_UNDIRECTEDNODE(NODE, GRAPH) { \
class_< Graph<NODE>, boost::noncopyable, std::shared_ptr< Graph<NODE> > >(#GRAPH, no_init) \
	 .def("create", std::make_shared< Graph<NODE> >) \
	 .staticmethod("create") \
	 .def("__iter__", iterator< Graph<NODE> >()) \
	 .def("manages", &Graph<NODE>::manages) \
	 .def("size", &Graph<NODE>::size); \
def("merge", &merge< Graph<NODE> >); \
class_< NODE, boost::noncopyable >(#NODE, init<>()) \
	.def( init< typename NODE::id_type >() ) \
	.def( init< std::shared_ptr< Graph<NODE> > >() ) \
	.def( init< std::shared_ptr< Graph<NODE> >, typename NODE::id_type >() ) \
	.add_property("ID", &NODE::ID) \
	.add_property("index", &NODE::get_index) \
	.def("add", &wrap_links<NODE>::add) \
	.def("remove", &wrap_links<NODE>::remove) \
	.def("clear", &NODE::clear) \
	.def("__iter__", iterator<NODE>()) \
	.def("contains", &NODE::contains) \
	.def("size", &NODE::size) \
	.def("join_index", &NODE::join_index) \
	.def("leave_index", &NODE::leave_index); }


//macros for input and output ports for message_nodes
#define BENOIT_TRANSLATE_INPUTPORT(PORT) { \
class_< PORT >(#PORT, init<const PORT&>()) \
	.add_property("address", &PORT::get_address) \
	.add_property("ready", &PORT::is_ready) \
	.def("pull", &PORT::pull); }

#define BENOIT_TRANSLATE_OUTPUTPORT(PORT) { \
class_< PORT >(#PORT, init<const PORT&>()) \
	.add_property("address", &PORT::get_address) \
	.add_property("ready", &PORT::is_ready) \
	.def("push", &PORT::push); }


//macro for DirectedNode and a corresponding graph, missing walk and find methods
//the graph is missing associative access to managed nodes
#define BENOIT_TRANSLATE_DIRECTEDNODE(NODE, GRAPH) { \
class_< Graph<NODE>, boost::noncopyable, std::shared_ptr< Graph<NODE> > >(#GRAPH, no_init) \
	.def("create", std::make_shared< Graph<NODE> >) \
	.staticmethod("create") \
	.def("__iter__", iterator< Graph<NODE> >()) \
	.def("manages", &Graph<NODE>::manages) \
	.def("size", &Graph<NODE>::size); \
def("merge", &merge< Graph<NODE> >); \
class_< LinkManager<NODE, typename NODE::input_type>, boost::noncopyable >("NODE ## _input_manager", no_init) \
	.def("__iter__", iterator< LinkManager<NODE, typename NODE::input_type> >()) \
	.def("__getitem__", &wrap_link_find< LinkManager<NODE, typename NODE::input_type> >::get, return_value_policy<return_by_value>()) \
	.def("contains", &LinkManager<NODE, typename NODE::input_type>::contains) \
	.def("size", &LinkManager<NODE, typename NODE::input_type>::size); \
class_< LinkManager<NODE, typename NODE::output_type>, boost::noncopyable >("NODE ## _output_manager", no_init) \
	.def("__iter__", iterator< LinkManager<NODE, typename NODE::output_type> >()) \
	.def("__getitem__", &wrap_link_find< LinkManager<NODE, typename NODE::output_type> >::get, return_value_policy<return_by_value>()) \
	.def("contains", &LinkManager<NODE, typename NODE::output_type>::contains) \
	.def("size", &LinkManager<NODE, typename NODE::output_type>::size); \
class_< NODE, boost::noncopyable >(#NODE, init<>()) \
	.def( init< typename NODE::id_type >() ) \
	.def( init< std::shared_ptr< Graph<NODE> > >() ) \
	.def( init< std::shared_ptr< Graph<NODE> >, typename NODE::id_type >() ) \
	.add_property("ID", &NODE::ID) \
	.add_property("index", &NODE::get_index) \
	.def("add_input", &wrap_input<NODE>::add) \
	.def("remove_input", &wrap_input<NODE>::remove) \
	.def("add_output", &wrap_output<NODE>::add) \
	.def("remove_output", &wrap_output<NODE>::remove) \
	.def("clear", &NODE::clear) \
	.def("clear_inputs", &NODE::clear_inputs) \
	.def("clear_outputs", &NODE::clear_outputs) \
	.def("join_index", &NODE::join_index) \
	.def("leave_index", &NODE::leave_index) \
	.def_readonly("inputs", &NODE::inputs) \
	.def_readonly("outputs", &NODE::outputs); }

#endif

