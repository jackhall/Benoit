#ifndef BenoitPort_h
#define BenoitPort_h

/*
    Benoit: a flexible framework for distributed graphs and spaces
    Copyright (C) 2011-2012  Jack Hall

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
    
    e-mail: jackwhall7@gmail.com
*/

#include <memory>
#include <boost/lockfree/spsc_queue.hpp>

namespace ben {
	
/* Both the user and Node use port objects to handle Buffers. Ports serve two main 
 * purposes. The first is to abstract away Buffer handling from Nodes, making both
 * classes easier to maintain. The second is to enforce the directedness of a link
 * by only allowing outputs to push data and inputs to pop. 
 *
 * These default implementations of the Port classes provide simple pass-throughs 
 * for Buffer method calls. More complex implementations could be written to store
 * extra data, check validity of pop operations or deal with custom Buffer classes
 * in a special way. 
 * 
 * Any replacement must have:
 * 	id_type typedef
 * 	complement_type typedef
 * 	construction_types typedef
 * 	get_address() method
 * 	clone() method
 * 	Constructor(id_type, construction_types...)
 * 	Constructor(complement_type, id_type)
 * 	copy construction/assignment (for the STL)
 */
	template<typename BUFFER>
	class Port {
    public:
		typedef unsigned int id_type;
        typedef typename BUFFER::signal_type signal_type;

    private:
        typedef Port self_type;
        id_type otherID;
		std::shared_ptr<BUFFER> buffer_ptr; 

    protected:
        template<typename... ARGS>
        Port(id_type ID, ARGS... args) : buffer_ptr(new BUFFER(args...)), otherID(ID) {}
        Port(const self_type& rhs, id_type ID) : buffer_ptr(rhs.buffer_ptr), otherID(ID) {}
		
		Port() = delete;
		Port(const self_type& rhs) = default;
		Port(self_type&& rhs) : buffer_ptr(std::move(rhs.buffer_ptr)), otherID(rhs.otherID) {}
		self_type& operator=(const self_type& rhs) = default;
		self_type& operator=(self_type&& rhs) { 
            if(this != &rhs) {
			    buffer_ptr = std::move( rhs.buffer_ptr );
                otherID = rhs.otherID;
            }
			return *this;
		}
		virtual ~Port() = default;
		bool push(const signal_type& signal) { return buffer_ptr->push(signal); } //take another look at const requirements
		bool pop(signal_type& signal) const { return buffer_ptr->pop(signal); }
    
	public:
		id_type get_address() const { return otherID; }
		bool is_ghost() const { return buffer_ptr.use_count() < 2; } //necessary but not sufficient for ghost :(
        bool operator==(const self_type& rhs) const { buffer_ptr == rhs.buffer_ptr; }
        bool operator!=(const self_type& rhs) const { return !operator==(rhs); }
	}; //class Port


	template<typename BUFFER> class InPort;
	template<typename BUFFER> class OutPort; //forward declaration to resolve circular typedefs


	template<typename BUFFER>
	class InPort : public Port<BUFFER> {
    /*This version allows popping but not pushing, enforcing the directedness of messages
     */
        typedef Port<BUFFER> base_type;

    public:
        typedef typename base_type::id_type id_type;
		typedef OutPort<BUFFER> complement_type;

        template<typename... ARGS>
        InPort(id_type sourceID, ARGS... args) : base_type(sourceID, args...) {}

        using base_type::pop;
        using base_type::get_address;
	}; //struct InPort
	
	template<typename BUFFER>
	class OutPort : public Port<BUFFER> {
    /*This version allows pushing but not popping, enforcing the directedness of messages
     */
        typedef Port<BUFFER> base_type;

    public:
        typedef typename base_type::id_type id_type;
		typedef InPort<BUFFER> complement_type;

        OutPort(const complement_type& complement, id_type targetID) 
            : base_type(complement, targetID) {}

        using base_type::push;
        using base_type::get_address;
    }; //struct OutPort


    /* Buffers are for the messages passed between Nodes. They're templated for both the
     * message type and buffer length. There are two partial specializations for buffer length B=1 
     * and B=2. 
     *
     * B=1 requires only one atomic element and no indexing. The other specializations each
     * have a B=1 Buffer as a member, and use it as a kind of staging area. Data can only be popped from
     * there, and so only the output port gives access to the rest of the Buffer code. Thus data races 
     * in the rest of the buffer are avoided, and Buffer B=1 encapsulates all atomic operations. 
     *
     * B=2 is special because it also needs no indexing. Larger Buffers are implemented as circular buffers. 
     *
     * Any replacement must match the public interface, which is the same for all specializations. 
     */
	template<typename SIGNAL, unsigned short SIZE> 
    struct Buffer : boost::lockfree::spsc_queue< SIGNAL, boost::lockfree::capacity<SIZE> > {
        typedef SIGNAL signal_type;
    };

} //namespace ben

#endif

