#ifndef State_h
#define State_h

/*
	(c) Jack Hall 2011, licensed under GNU LGPL v3
	State is a data structure meant to store batch and time-varying data in a neural network.
	For the purposes of backpropagation, signal data must be stored for a given time step until
	the network backpropagates through that time step. Once this happens that data may be safely
	overwritten. 
	
	Requirements/Characteristics:
	- Must not require memory reallocations during the normal funtion of a net, including training.
	- Should be able to handle any data type that an represent fractions of a whole number.
	- The interface should consider time to be relative.
	- It should be possible to pass either the entire container or a single time step by reference/pointer.
	- Should conform as much as possible to the STL style for containers.
	- Basic array operations: =array =scalar, += -= *= /=, == !=, ++time --time
	- Both automatic and manual memory management should be available.
*/

#include <iostream>

template<typename T=float>
class State { 
/*
	For now, State is implemented with a flattened dynamic array. Using an STL vector is a 
	possibility. A vector would make memory- and exception-safety easier to achieve, but
	vectors may reserve a great deal more capacity than they need. Since there are so many
	of these objects in a network (4 per Neuron), memory requirements may increase too much.
	
	State is a matrix with samples by row and time steps by column. The time dimension is 
	circular, such that with 4 time steps, going back one step would be the same as going
	forward three steps. This arrangement avoids the constant push_back, pop_front costs while
	not requiring all time steps to be copied forward at each step. Also, pointers and iterators
	will stay valid longer. 
	
	- A pointer of the template type to the beginning of the array.
	- Integers for the number of samples and timesteps, as well as an integer tracking the 
		currently-reserved capacity and another giving the number of the current step.
	- Both iterator and const_iterator are defined. They are identical aside from the const-ness
		of the references returned by the access functions and operators. See State::iterator
		for more detailed comments.
	- Unlike most classes in Benoit, both assignment and copy constructing are allowed. 
	- Both const and non-const versions of operator[] and operator() are defined. operator[] 
		treats State as a flattened array (low-level access) and operator() treats State as 
		a partially circular matrix (high-level access)
	- reserve, trim, capacity, and empty provide manual memory management.
	- Overloaded ++ and -- operators increment or decrement the current time step.
	- samples, steps, size and resize offer high-level storage management.
	- firstStample and lastSample help create iterators.
	- initializeStep sets all the values in a given column. 
	- zero sets all elements in State to zero.
	- begin and end act on the given time step like their STL analogs.
*/

private:
	T* mpData;
	unsigned int mnSteps, mnSamples, mnCapacity, mnCurrentStep;
	inline T* firstSample() { return &mpData[mnSamples*(mnCurrentStep-1)]; }
	inline T* lastSample() { return firstSample() + mnSamples-1; }

public:
	class iterator { 
	/*
		The State::iterator classes function as weak smart pointers for individual columns
		of a State object. In Benoit, a column represents the data from one time step, and
		so will be passed around a lot. These iterator classes make it possible to avoid 
		passing all this data by value. 
		- A member of the State namespace, similar to how a vector<double>::iterator is part
			of the vector<double> namespace. Similarity with the STL is good, and this way
			a class instance of iterator is stenciled out whenever a corresponding version
			of State is needed.
		- Pointers the current, first and last elements are needed for pointing and bounds 
			checking.
		- A boolean value tracks the bounds status to speed bounds-checking. Bounds-checking
			calculations are done during increment and decrement only, potentially saving 
			cycles if the iterator is derefenced more often than incremented.
		- A pair of inline functions query the bounds status. Since the dereference operator
			throws if the iterator is out of bounds, this could allow the programmer to avoid
			catching too many exceptions.
		- Increment and decrement operators move the iterator and perform bounds-checking.
		- operator[] allows random access counted from the first element in the column. This
			also moves the iterator's current position and performs bounds-checking.
		- operator== and operator!= check only whether the iterators are pointing to the same 
			place. It does not check whether they have equivalent bounds. 
		- Add +,- operators later to emulate pointer arithmetic?
	*/
	private:
		T* mpFirst;	//beginning element of the column of State
		T* mpLast;	//end element of the column (not the past-end element!)
		T* mpCurrent;
		bool mbOutOfBounds;
	
	public:
		iterator() 
			: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbOutOfBounds(false) {}
		iterator(	const T* pFirst,
					const T* pLast,
					const T* pCurrent) 
			: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbOutOfBounds(false) {}
		inline bool inBounds() { return !mbOutOfBounds; }	//have both of these for convenience
		inline bool outofBounds() {return mbOutOfBounds; }	
		inline T& operator*(){
			if(mbOutOfBounds) throw "Dereferenced StateIterator out of bounds";
			else return *mpCurrent; }
		T& operator[](const unsigned int nIndex);
		iterator& operator++();
		iterator& operator--();
		iterator operator++(int);
		iterator operator--(int);
		bool operator==(const iterator& cTwo) { return mpCurrent==cTwo.mpCurrent; }
		bool operator!=(const iterator& cTwo) { return mpCurrent!=cTwo.mpCurrent; }
		//pointer arithmetic
		//[] dereference
		//comparison > < => =<
		//compound assignment 
	}; //class iterator
	
	class const_iterator {
	//see State<T>::iterator comments
	//need a way to cast between const and nonconst?
	//differences: 
	//	- dereferencing with * or [] returns a const reference
	private:
		T* mpFirst;
		T* mpLast;
		T* mpCurrent;
		bool mbOutOfBounds;
	
	public:
		const_iterator() 
			: mpFirst(NULL), mpLast(NULL), mpCurrent(NULL), mbOutOfBounds(false) {}
		const_iterator(	const T* pFirst,
						const T* pLast,
						const T* pCurrent) 
			: mpFirst(pFirst), mpLast(pLast), mpCurrent(pCurrent), mbOutOfBounds(false) {}
		inline bool inBounds() { return !mbOutOfBounds; }
		inline bool outofBounds() {return mbOutOfBounds; }
		inline const T& operator*(){
			if(mbOutOfBounds) throw "Dereferenced StateIterator out of bounds";
			else return *mpCurrent; }
		const T& operator[](const unsigned int nIndex);
		const_iterator& operator++();
		const_iterator& operator--();
		const_iterator operator++(int);
		const_iterator operator--(int);
		bool operator==(const const_iterator& cTwo) { return mpCurrent==cTwo.mpCurrent; }
		bool operator!=(const const_iterator& cTwo) { return mpCurrent!=cTwo.mpCurrent; }
	}; //class const_iterator
	
	//----------- constructors, destructor, plain assignment -----------
	State();
	State(	const unsigned int nSamples,
			const unsigned int nSteps = 1);
	State(const State& cSource);
	State& operator=(const State& cSource);
	~State();
	
	//----------- overloaded operators ---------------
	T& operator()(	const unsigned int nSample, 
					const unsigned int nStepBack = 0);
	const T& operator()(const unsigned int nSample, 
						const unsigned int nStepBack = 0) const; //const and non-const versions
	inline T& operator[](const unsigned int nIndex)
		{ return mpData[nIndex]; }
	inline const T& operator[](const unsigned int nIndex) const //const and non-const versions
		{ return mpData[nIndex]; }
	bool operator==(const State& cTwo); 		//checks all elements; check pointers instead?	
	inline bool operator!=(const State& cTwo) { return !(*this==cTwo); }
	State& operator+=(const State& cNew);
	State& operator-=(const State& cNew);
	State& operator*=(const State& cNew);
	State& operator/=(const State& cNew);
	inline State& operator++()
		{	++mnCurrentStep;
			if(mnCurrentStep == mnSteps) mnCurrentStep = 0;
			return *this; }
	inline State& operator--()
		{	if(mnCurrentStep == 0) mnCurrentStep = mnSteps;
			--mnCurrentStep;
			return *this; }
	State operator++(int);
	State operator--(int);
	
	template<typename U>
	friend ostream& operator<<(ostream& out, const State<U>& cState);
	
	//---------- public methods -------------------
	//////// iterator-related //////////
	iterator begin(unsigned int nStepBack=0);
	iterator end(unsigned int nStepBack=0);  //need const versions?
	
	//////// general management ////////
	inline bool empty() { return size()==0; }
	State& zero();
	State& initializeStep(const T tBias); //also overload operator= to take type T?
	State& trim();
	State& reserve(const unsigned int nCapacity);
	inline unsigned int size() const { return mnSteps*mnSamples; }
	inline unsigned int capacity() const { return mnCapacity; }
	State& resize(	const unsigned int nSamples,
					const unsigned int nSteps = 1);
	inline unsigned int samples() const { return mnSamples; }
	inline unsigned int steps() const { return mnSteps; }
}; //class State

#include "State.cpp"

#endif

