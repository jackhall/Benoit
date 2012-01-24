#ifndef BenoitLink_cpp
#define BenoitLink_cpp

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

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
*/

namespace ben {
	//=================== CTOR ==========================
	template<typename W, typename S>
	Link<W,S>::Link(Index<W,S>* const pIndex,
			const unsigned int nOrigin, 
		   	const unsigned int nTarget, 
		   	const W& wWeight)
		: index(pIndex), origin(nOrigin), target(nTarget), weight(wWeight) {
		//add pointer to origin Node
		index->find(origin)->add_output(this); //index is NULL?
		mark[0] = false;
		mark[1] = false;
	} //constructor
	
	template<typename W, typename S>
	Link<W,S>::Link(Link&& rhs) 
		: origin(rhs.origin), target(rhs.target), index(rhs.index), weight(rhs.weight) {
		if(index != NULL) index->find(origin)->update_output(&rhs, this);
		rhs.index = NULL;
		buffer[0] = rhs.buffer[0];
		buffer[1] = rhs.buffer[1];
		mark[0] = rhs.mark[0];
		mark[1] = rhs.mark[1];
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
		if(mark[1]) { 
			buffer[1] = data;
			mark[1] = false;
		} else {
			buffer[0] = data;
			mark[1] = true;
		}
	} //push
	
	//add readiness checks later? probably not, for speed's sake
	template<typename W, typename S>
	S Link<W,S>::pull() {
		//reads next data packet from buffer and moves the read mark to the next buffer space
		if(mark[0]) {
			mark[0] = false;
			return buffer[1];
		} else {
			mark[0] = true;
			return buffer[0];
		}
	} //pull
	
	template<typename W, typename S>
	void Link<W,S>::update_index(Index<W,S>* const pIndex) {
		index = pIndex;
	} //update_index
	
} //namespace ben

#endif
