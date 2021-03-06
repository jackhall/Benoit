Benoit is a heavily templated (and therefore extremely flexible) framework for distributed networks and graphs. Instead of treating a graph as a collection that owns its nodes, node ownership is specified by client code and the nodes have shared ownership of the graph. This implementation is easier to integrate into other code, and potentially thread-safe. I plan to use it as the basis for a multithreaded neural networks library, among other things. 

Benoit is licensed under the GPL v3, copyright John Wendell Hall 7/23/2011. Current release date: 9/25/2013. 
Version numbers follow the semantic versioning scheme.
The current version is: 2.2.0 

To test, use make test_singleton and test_graph in the test directory and run the resulting executables.

The class templates Benoit defines are as follows:
Graph<typename NODE>: serves as an index to manage the distributed nodes of the graph, but does not own them. 
DirectedNode<typename INPUT, typename OUTPUT>: the node of a directed graph. The INPUT and OUTPUT types are Ports or Paths as described below.
UndirectedNode<typename PATH>: the node of an undirected graph. PATH types are described below.
InPort<typename BUFFER>, OutPort<typename BUFFER>: paired types that share ownership of a Buffer. For a given link, the source node owns an OutPort and the target node owns an InPort.
Buffer<typename SIGNAL, size_t SIZE>: implements a buffer to pass values between port objects.
Path<typename VALUE>: similar to Ports, except they store values instead of sending messages. Paired with itself. 

All classes exist in the "ben" namespace. Since Benoit is a header-only library, all you have to do is #include Benoit.h to use it. Interface and implementation details are documented in the source files. Since these files are related to one another through type parameterization, they are completely modular. There is no reason not to define your own Port type, for instance, if you don't like the default. The header for each class template describes which parts of its interface are required by other class templates.

Python bindings exist and are demonstrated in src/benoitpy.cpp. It may also be necessary to alter src/python_utils.h. These files make use of boost.python. See the makefile in the home directory for compilation instructions (or just type "make" if everything is in order).

Coming soon: type trait checking for each class currently included in the library, to aid customization.

My email is jackwhall7@gmail.com. 

