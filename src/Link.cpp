#ifndef BenoitLink_cpp
#define BenoitLink_cpp

/*
    Benoit: a flexible framework for distributed graphs
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

namespace ben {
	//=================== CTOR ==========================
	template<typename W, typename S>
	Link<W,S>::Link(Index<W,S>* const pIndex,
			const unsigned int nOrigin, 
		   	const unsigned int nTarget, 
		   	const W& wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight), mark(0) {
		//add pointer to origin Node
		index->find(origin)->add_output(this); //index is NULL?
	} //constructor
	
	template<typename W, typename S>
	Link<W,S>::Link(Link&& rhs) 
		: origin(rhs.origin), target(rhs.target), index(rhs.index), weight(rhs.weight), mark(rhs.mark) {
		if(index != NULL) index->find(origin)->update_output(&rhs, this);
		rhs.index = NULL;
		buffer[0] = rhs.buffer[0];
		buffer[1] = rhs.buffer[1];
	} //copy constructor
	
	//===================== DTOR ========================
	template<typename W, typename S>
	Link<W,S>::~Link() {
		//remove pointer at origin Node
		if(index != NULL) index->find(origin)->remove_output(this);
	} //destructor
	
	//=================== METHODS ==========================
	template<typename W, typename S>
	void Link<W,S>::push(const S& data) { 
		//writes data packet to buffer and moves the write mark to the next buffer space
		switch(mark) {
			case 0: buffer[0] = data; 	mark = 5;	break; //nothing in buffer, not ready
			case 1: buffer[1] = data;	mark = 4;	break; 
			case 2: buffer[0] = data;	mark = 7;	break; //one value in buffer, not ready
			case 3: buffer[1] = data;	mark = 6;	break;
			case 4: buffer[0] = data;	mark = 7;	break; //one value in buffer, ready
			case 5: buffer[1] = data;	mark = 6;	break;
			case 6: buffer[0] = data;	mark = 7;	break; //two values in buffer, ready
			case 7: buffer[1] = data;	mark = 6;	break;
			
			default: std::cerr << "Invalid ben::Link state" << std::endl;
		}
	} //push
	
	//add readiness checks later? probably not, for speed's sake
	template<typename W, typename S>
	S Link<W,S>::pull() {
		//reads next data packet from buffer and moves the read mark to the next buffer space
		switch(mark) {
			case 0: return buffer[1]; //nothing in buffer, not ready
			case 1: return buffer[0]; 
			case 2: return buffer[0]; //one value in buffer, not ready
			case 3: return buffer[1]; 
			case 4: mark = 1;	return buffer[1]; //one value in buffer, ready
			case 5: mark = 0;	return buffer[0]; 
			case 6: mark = 3;	return buffer[0]; //two values in buffer, ready
			case 7: mark = 2;	return buffer[1]; 
			default: std::cerr << "Invalid ben::Link state" << std::endl;
		}
	} //pull
	
	template<typename W, typename S>
	bool Link<W,S>::ready() const {
		return mark > 3;
	}
	
	template<typename W, typename S>
	unsigned int Link<W,S>::values() const {
		if(mark < 2) return 0;
		else if(mark < 6) return 1;
		else return 2;
	}
	
	template<typename W, typename S>
	S Link<W,S>::pop() {
		switch(mark) {
			case 2: mark = 1; 	return buffer[1]; //not ready, pointing at other value
			case 3: mark = 0;	return buffer[0];
			case 4: mark = 0;	return buffer[0]; //not ready, pointing at same value
			case 5: mark = 1;	return buffer[1];
			case 6: mark = 5;	return buffer[0]; //still ready, but pointing at other value
			case 7: mark = 4;	return buffer[1];
			default: break;
		}
	}
	
	template<typename W, typename S>
	void Link<W,S>::update_index(Index<W,S>* const pIndex) {
		index = pIndex;
	} //update_index
	
} //namespace ben

#endif
