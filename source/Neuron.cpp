#ifndef Neuron_cpp
#define Neuron_cpp

template<typename T>
State<T>& Neuron<T>::add(Connection_base* pConn) {
	Neuron<T>* pSource = static_cast<Neuron<T>*>(pConn->getSource());
	Connection<T>* pIn = static_cast<Connection<T>*>(pConn);
	typename State<T>::iterator iOutput = pSource->iOutput.begin();
	typename State<T>::iterator iSignal = msSignal.begin();
	while(iSignal.inBounds()) {
		*iSignal += pIn->op(iOutput);
		++iSignal;
		++iOutput;
	}
	return msSignal;
}

template<typename T>
State<T>& Neuron<T>::multiply(Connection_base* pConn) {
	Neuron* pSource = static_cast<Neuron*>(pConn->getSource());
	Connection<T>* pIn = static_cast<Connection<T>*>(pConn);
	typename State<T>::iterator iOutput = pSource->iOutput.begin();
	typename State<T>::iterator iSignal = msSignal.begin();
	while(iSignal.inBounds()) {
		*iSignal *= pIn->op(iOutput);
		++iSignal;
		++iOutput;
	}
	return msSignal;
}

template<typename T>
State<T>& Neuron<T>::subtract(Connection_base* pConn) {
	Neuron* pSource = static_cast<Neuron*>(pConn->getSource());
	Connection<T>* pIn = static_cast<Connection<T>*>(pConn);
	typename State<T>::iterator iOutput = pSource->iOutput.begin();
	typename State<T>::iterator iSignal = msSignal.begin();
	while(iSignal.inBounds()) {
		*iSignal -= pIn->op(iOutput);
		++iSignal;
		++iOutput;
	}
	return msSignal;
}

template<typename T>
State<T>& Neuron<T>::addD(Connection_base* pConn) {
	return msBuffer;
}

template<typename T>
State<T>& Neuron<T>::multiplyD(Connection_base* pConn) {
	return msBuffer;
}

template<typename T>
State<T>& Neuron<T>::subtractD(Connection_base* pConn) {
	return msBuffer;
}

template<typename T>
State<T>& Neuron<T>::tanh() { 
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iOutput = msOutput.begin();
	while(iSignal.inBounds()) {
		*iOutput = tanh(*iSignal); //tanh + templates?
		++iOutput;
		++iSignal;
	}
	return msOutput;
}

template<typename T>
State<T>& Neuron<T>::linear() { 
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iOutput = msOutput.begin();
	while(iSignal.inBounds()) {
		*iOutput = *iSignal;
		++iOutput;
		++iSignal;
	}
	return msOutput;
}

template<typename T>
State<T>& Neuron<T>::rbf() { 
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iOutput = msOutput.begin();
	while(iSignal.inBounds()) {
		iOutput = exp(-((*iSignal)^2.0)); //exp doesn't take templates?
		++iOutput;
		++iSignal;
	}
	return msOutput;
}

template<typename T>
State<T>& Neuron<T>::tanhD() { //use argument to step back?
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iError = msError.begin();
	typename State<T>::iterator iBuffer = msBuffer.begin();
	while(iError.inBounds()) {
		*iError = (*iBuffer) * (1.0/cosh(*iSignal))^2.0; //cosh doesn't take templates?
		++iSignal;
		++iError;
		++iBuffer;
	}
	return msError;
}

template<typename T>
State<T>& Neuron<T>::linearD() { //use argument to step back?
	typename State<T>::iterator iError = msError.begin();
	typename State<T>::iterator iBuffer = msBuffer.begin();
	while(iError.inBounds()) {
		*iError = *iBuffer;
	}
	return msError;
}

template<typename T>
State<T>& Neuron<T>::rbfD() { //use argument to step back?
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iError = msError.begin();
	typename State<T>::iterator iBuffer = msBuffer.begin();
	while(iError.inBounds()) {
		*iError = (*iBuffer) * -2.0*(*iSignal)*exp(-((*iSignal)^2.0)); //exp + templates?
	}
	return msError;
}

template<typename T> 
Neuron<T>::Neuron(	const char chActivationFcn,
					const char chOperator,
					const unsigned int nSamples,
					const unsigned int nTimeSteps,
					const T tBias,
					const bool bTrainable) 
	: Neuron_base(bTrainable), mtBias(tBias), 
		msSignal(nSamples, nTimeSteps), msOutput(nSamples, nTimeSteps),
		msError(nSamples), msBuffer(nSamples) {
	switch(chOperator) {
		case "+":
			op = add;
			opderiv = addD;
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
			throw "Invalid operator for Neuron";
	}
	
	switch(chActivationFcn) {
		case "t":
			activationfcn = tanh;
			activationfcnderiv = tanhD;
			break;
		case "l":
			activationfcn = linear;
			activationfcnderiv = linearD;
			break;
		case "r":
			activationfcn = rbf;
			activationfcnderiv = rbfD;
			break;
		default:
			throw "Invalid activation function for Neuron";
	}	
}

template<typename T>
Neuron<T>::Neuron(const Neuron<T>& cSource) 
	: Neuron_base(cSource), mtBias(cSource.mtBias),
		msSignal(cSource.msSignal.samples(),cSource.msSignal.steps()), 
		msOutput(cSource.msOutput.samples(),cSource.msOutput.steps()),
		msBuffer(cSource.msBuffer.samples()), 
		msError(cSource.msError.samples()) {
	if(cSource.op==&(cSource.add)) {
		op = add;
		opderiv = addD;
	} else if(cSource.op==&(cSource.multiply)) {
		op = multiply;
		opderiv = multiplyD;
	} else if(cSource.op==&(cSource.subtract)) {
		op = subtract;
		opderiv = subtractD;
	} else throw "Invalid operator for Neuron";
	
	if(cSource.activationfcn==&(cSource.tanh)) {
		activationfcn = tanh;
		activationfcnderiv = tanhD;
	} else if(cSource.activationfcn==&(cSource.linear)) {
		activationfcn = linear;
		activationfcnderiv = linearD;
	} else if(cSource.activationfcn==&(cSource.rbf)) {
		activationfcn = rbf;
		activationfcnderiv = rbfD;
	} else throw "Invalid activation function for Neuron";
}

template<typename T>
Node& Neuron<T>::fire() { //not finished (step sequence)
	using namespace std;
	msSignal.initializeStep(mtBias);
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		op(*it);
		(*it)->mbSignalFlag = false;
		++it;
	}
	
	activationfcn();
	
	it = mqConnOut.begin();
	ite = mqConnOut.end();
	while(it!=ite) {
		(*it)->mbSignalFlag = true;
		++it;
	}
	return *this;
}

template<typename T>
Node& Neuron<T>::backPropagate() { //not finished (step sequence)
	using namespace std;
	msBuffer.initializeStep(0.0);
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	while(it!=ite) {
		opderiv(*it);
		(*it)->mbErrorFlag = false;
		++it;
	}
	
	activationfcnderiv();
	
	it = mqConnIn.begin();
	ite = mqConnIn.end();
	while(it!=ite) {
		(*it)->mbErrorFlag = true;
		++it;
	}
	return *this;
}

template<typename T>
Connection<T>& Neuron<T>::addInput(	const Node* pNewIn,
									const char chOperator,
									const unsigned int nTimeDelay) {
	new Connection<T>(pNewIn, this, chOperator, nTimeDelay);
}

template<typename T>
Connection<T>& Neuron<T>::addOutput(const Node* pNewOut,
									const char chOperator,
						  			const unsigned int nTimeDelay) {
	new Connection<T>(this, pNewOut, chOperator, nTimeDelay);
}

template<typename T>
ostream& Neuron<T>::print(ostream &out) const {

	return out;
}

#endif

