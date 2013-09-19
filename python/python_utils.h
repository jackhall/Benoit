#ifndef python_utils_h
#define python_utils_h

#include <memory>
#include <string>
#include <sstream>
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

	
	//compare operators for graph
	template<typename T>
	struct wrap_compare {
		static bool equal(T& x, T& y) {
			return &x == &y;
		}

		static bool not_equal(T& x, T& y) {
			return &x != &y;
		}
	};

	//print operator
	//template<typename... ARGS>
	//struct wrap_print {};

	//template<typename... ARGS>
	//struct wrap_print< Path<ARGS> > {
	//	static std::string call(Path<ARGS>& x) {
	//		std::ostringstream convert;
	//		convert << "ID = " << x.get_address() << "value = " << x.get_value() << std::endl;
	//		return convert.str();
	//	}
	//};
	
} //namespace ben


//These macros expands a path type and names for node and graph into a boost.python translation.
//Directions for this and the following macros:
//TO USE: pass in type names for the appropriate arguments, and type-compatible strings for the rest
//NOTE: use type names that also work as type names in python!
//NOTE:these macros should all be used either inside the "ben" namespace or with 
//a "using namespace ben;" active
//NOTE:each use of each macro should be unique to avoid naming collisions

//macro for UndirectedNode, missing walk and find, including a corresponding graph
//the graph is missing associative access to managed nodes
#define BENOIT_EXPOSE_UNDIRECTEDGRAPH(PATH, NODE, GRAPH) { \
class_< PATH >(#PATH, init<const PATH&>()) \
	.def( self==self ) \
	.def( self!=self ) \
	.add_property("value", &PATH::get_value, &PATH::set_value) \
	.add_property("address", &path::get_address); \
class_< Graph< UndirectedNode<PATH> >, boost::noncopyable, std::shared_ptr< Graph< UndirectedNode<PATH> > > >(#GRAPH, no_init) \
	.def("__init__", make_constructor(std::make_shared< Graph< UndirectedNode<PATH> > >)) \
	.def("__eq__", &wrap_compare< Graph< UndirectedNode<PATH> > >::equal) \
	.def("__ne__", &wrap_compare< Graph< UndirectedNode<PATH> > >::not_equal) \
	.def("manages", &Graph< UndirectedNode<PATH> >::manages) \
	.def("__len__", &Graph< UndirectedNode<PATH> >::size); \
def("merge", &merge< Graph< UndirectedNode<PATH> > >); \
class_< UndirectedNode<PATH>, boost::noncopyable >(#NODE, init<>()) \
	.def( init< typename UndirectedNode<PATH>::id_type >() ) \
	.def( init< std::shared_ptr< Graph< UndirectedNode<PATH> > > >() ) \
	.def( init< std::shared_ptr< Graph< UndirectedNode<PATH> > >, typename UndirectedNode<PATH>::id_type >() ) \
	.def("__eq__", &wrap_compare< UndirectedNode<PATH> >::equal) \
	.def("__ne__", &wrap_compare< UndirectedNode<PATH> >::not_equal) \
	.add_property("ID", &UndirectedNode<PATH>::ID) \
	.def("is_managed", &UndirectedNode<PATH>::is_managed) \
	.add_property("index", &UndirectedNode<PATH>::get_index) \
	.def("add", &wrap_links<UndirectedNode<PATH>>::add) \
	.def("remove", &wrap_links<UndirectedNode<PATH>>::remove) \
	.def("clear", &UndirectedNode<PATH>::clear) \
	.def("__iter__", iterator<UndirectedNode<PATH>>()) \
	.def("contains", &UndirectedNode<PATH>::contains) \
	.def("__len__", &UndirectedNode<PATH>::size) \
	.def("join_index", &UndirectedNode<PATH>::join_index) \
	.def("leave_index", &UndirectedNode<PATH>::leave_index); }

//GRAPH.def("__iter__", iterator< Graph< UndirectedNode<PATH> > >()) \


//macro for DirectedNode and a corresponding graph, missing walk and find methods
//the graph is missing associative access to managed nodes
#define BENOIT_EXPOSE_DIRECTEDGRAPH(INPUT, OUTPUT, NODE, GRAPH) { \
class_< INPUT >(#INPUT, init<const INPUT&>()) \
	.def( self==self ) \
	.def( self!=self ) \
	.add_property("address", &INPUT::get_address) \
	.def("pull", &INPUT::pull); \
class_< OUTPUT >(#OUTPUT, init<const OUTPUT&>()) \
	.def( self==self ) \
	.def( self!=self ) \
	.add_property("address", &OUTPUT::get_address) \
	.def("push", &OUTPUT::push); \
class_< Graph< DirectedNode<INPUT, OUTPUT> >, boost::noncopyable, std::shared_ptr< Graph< DirectedNode<INPUT, OUTPUT> > > >(#GRAPH, no_init) \
	.def("__init__", make_constructor(std::make_shared< Graph< DirectedNode<INPUT, OUTPUT> > >)) \
	.def("__eq__", &wrap_compare< Graph< DirectedNode<INPUT, OUTPUT> > >::equal) \
	.def("__ne__", &wrap_compare< Graph< DirectedNode<INPUT, OUTPUT> > >::not_equal) \
	.def("manages", &Graph< DirectedNode<INPUT, OUTPUT> >::manages) \
	.def("__len__", &Graph< DirectedNode<INPUT, OUTPUT> >::size); \
def("merge", &merge< Graph< DirectedNode<INPUT, OUTPUT> > >); \
class_< LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT>, boost::noncopyable >("NODE ## _input_manager", no_init) \
	.def("__eq__", &wrap_compare< LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT> >::equal) \
	.def("__ne__", &wrap_compare< LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT> >::not_equal) \
	.def("__iter__", iterator< LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT> >()) \
	.def("__getitem__", &wrap_link_find< LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT> >::get, return_value_policy<return_by_value>()) \
	.def("contains", &LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT>::contains) \
	.def("__len__", &LinkManager<DirectedNode<INPUT, OUTPUT>, INPUT>::size); \
class_< LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT>, boost::noncopyable >("NODE ## _output_manager", no_init) \
	.def("__eq__", &wrap_compare< LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT> >::equal) \
	.def("__ne__", &wrap_compare< LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT> >::not_equal) \
	.def("__iter__", iterator< LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT> >()) \
	.def("__getitem__", &wrap_link_find< LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT> >::get, return_value_policy<return_by_value>()) \
	.def("contains", &LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT>::contains) \
	.def("__len__", &LinkManager<DirectedNode<INPUT, OUTPUT>, OUTPUT>::size); \
class_< DirectedNode<INPUT, OUTPUT>, boost::noncopyable >(#NODE, init<>()) \
	.def( init< typename DirectedNode<INPUT, OUTPUT>::id_type >() ) \
	.def( init< std::shared_ptr< Graph< DirectedNode<INPUT, OUTPUT> > > >() ) \
	.def( init< std::shared_ptr< Graph< DirectedNode<INPUT, OUTPUT> > >, typename DirectedNode<INPUT, OUTPUT>::id_type >() ) \
	.def("__eq__", &wrap_compare< DirectedNode<INPUT, OUTPUT> >::equal) \
	.def("__ne__", &wrap_compare< DirectedNode<INPUT, OUTPUT> >::not_equal) \
	.add_property("ID", &DirectedNode<INPUT, OUTPUT>::ID) \
	.def("is_managed", &DirectedNode<INPUT, OUTPUT>::is_managed) \
	.add_property("index", &DirectedNode<INPUT, OUTPUT>::get_index) \
	.def("add_input", &wrap_input<DirectedNode<INPUT, OUTPUT>>::add) \
	.def("remove_input", &wrap_input<DirectedNode<INPUT, OUTPUT>>::remove) \
	.def("add_output", &wrap_output<DirectedNode<INPUT, OUTPUT>>::add) \
	.def("remove_output", &wrap_output<DirectedNode<INPUT, OUTPUT>>::remove) \
	.def("clear", &DirectedNode<INPUT, OUTPUT>::clear) \
	.def("clear_inputs", &DirectedNode<INPUT, OUTPUT>::clear_inputs) \
	.def("clear_outputs", &DirectedNode<INPUT, OUTPUT>::clear_outputs) \
	.def("join_index", &DirectedNode<INPUT, OUTPUT>::join_index) \
	.def("leave_index", &DirectedNode<INPUT, OUTPUT>::leave_index) \
	.def_readonly("inputs", &DirectedNode<INPUT, OUTPUT>::inputs) \
	.def_readonly("outputs", &DirectedNode<INPUT, OUTPUT>::outputs); }

//GRAPH.def("__iter__", iterator< Graph< DirectedNode<INPUT, OUTPUT> > >()) \

//issues with python version: 
//	graph iterators don't work, probably because they require node copying (removed)
//	message_node.add_input sometimes fails for no discernible reason(? might be fixed)
//	python interpreter sometimes segfaults on exit(? might be fixed)

#endif

