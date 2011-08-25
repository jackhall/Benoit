#ifndef Connection_cpp
#define Connection_cpp

//(c) Jack Hall 2011, licensed under GNU LGPL v3

template<typename T>
Connection<T>::Connection(	Neuron<T>* pSource,		//Neuron and Connection must share a template type
							Neuron<T>* pTarget,
			   				const char chOperator,
			   				const unsigned int nTimeDelay,
			   				const T tWeight,
			   				const bool bTrainable) 
	: Connection_base(pSource, pTarget, bTrainable), 
		mtWeight(tWeight), mnTimeDelay(nTimeDelay) {
	setOperator(chOperator);
}

template<typename T>
Connection<T>::Connection(	const unsigned int nSource,
							const unsigned int nTarget,
			   				const char chOperator,
			   				const unsigned int nTimeDelay,
			   				const T tWeight,
			   				const bool bTrainable) 
	: Connection_base(nSource, nTarget, bTrainable),  //same because Connection_base can deal with IDs and pointers
		mtWeight(tWeight), mnTimeDelay(nTimeDelay) {
	setOperator(chOperator);
}

template<typename T>
void Connection<T>::setOperator(const char chOperator) {
	switch(chOperator) {	//this is the only code that alters
		case "+":			//the operator function pointers,
			op = add;		//ensuring that the backward and forward
			opderiv = addD;	//versions match
			break;
		case "*":
			op = multiply;
			opderiv = multiplyD;
			break;
		case "-":
			op = subtract;
			opderiv = subtractD;
			break;
		default:
			throw "Invalid operator for Connection";
	}
}

template<typename T>
ostream& Connection<T>::print(ostream& out) const { //not finished
	//weight, operator, time delay
	//call Connection_base::print
	return out;
}

#endif

