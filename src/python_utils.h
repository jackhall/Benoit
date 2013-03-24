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

#endif
