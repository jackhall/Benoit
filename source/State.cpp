#ifndef State_cpp
#define State_cpp

//----------- constructors, destructor, assignment -------------
template<typename T>
State<T>::State() {
}

template<typename T>
State<T>::State(const unsigned int nSamples,
				const unsigned int nSteps) {
}

template<typename T>
State<T>::State(const State& cSource) {
}

template<typename T>
State<T>&  State<T>::operator=(const State& cSource) {
}

template<typename T>
State<T>::~State() {
}

//----------- overloaded increment/decrement operators ----------
template<typename T>
State<T>& State<T>::operator++() {
}

template<typename T>
State<T>& State<T>::operator--() {
}

template<typename T>
State<T>  State<T>::operator++(int) {
}

template<typename T>
State<T>  State<T>::operator--(int) {
}
	
//----------- memory management/STL-style methods --------------

template<typename T>
State<T>& State<T>::resize(	const unsigned int nSamples,
							const unsigned int nSteps) {
}

template<typename T>
State<T>::iterator  State<T>::begin(unsigned int nStepBack) {
}

template<typename T>
State<T>::iterator  State<T>::end(unsigned int nStepBack) {
}

//------------------- iterator methods ---------------------
///////// constructors, destructor, assignment

///////// overloaded operators (pointer arithmetic included)

///////// STL-style methods for smart pointers

#endif
