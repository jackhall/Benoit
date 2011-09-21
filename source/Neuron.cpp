#ifndef Neuron_cpp
#define Neuron_cpp

//(c) Jack Hall 2011, licensed under GNU LGPL v3

template<typename T>
State<T>& Neuron<T>::add(Connection_base* pConn) {
	//cast pointers from interface types to implementation types. The constructors
	//of the implementations require ensure other children of the Benoit interface 
	//are not used, so type checking is not needed.
	Neuron<T>* pSource = static_cast<Neuron<T>*>(pConn->getSource());
	Connection<T>* pIn = static_cast<Connection<T>*>(pConn);
	typename State<T>::iterator iOutput = pSource->iOutput.begin();		//get iterator from input Neuron
	typename State<T>::iterator iSignal = msSignal.begin();				//iterator from current signal State
	while(iSignal.inBounds()) {
		*iSignal += pIn->op(iOutput);		//call the Connection's operator function pointer
		++iSignal;
		++iOutput;
	}
	return msSignal;
}

template<typename T>
State<T>& Neuron<T>::multiply(Connection_base* pConn) {
	//see Neuron::add for helpful code comments
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
	//see Neuron::add for helpful code comments
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
State<T>& Neuron<T>::addD(Neuron<T>* pSource, Connection<T>* pConn) {			//use argument to step back?
	//edit arguments later?
	//the operator derivatives both calculate the derivative and add it to the error gradient, 
	//as per the chain rule
	
	//get iterators from input's Buffer and own Error state
	typename State<T>::iterator iBuffer = pSource->msBuffer.begin();
	typename State<T>::iterator iError = msError.begin();
	while(iError.inBounds()) {
		*iBuffer += pConn->opderiv(iError);		//call the derivative of Connections's operator
		++iError;
		++iBuffer;
	}
	return pSource->msBuffer;
}

template<typename T>
State<T>& Neuron<T>::multiplyD(Neuron<T>* pSource, Connection<T>* pConn) {	//use argument to step back?
	//see Neuron::addD for helpful code comments
	//multiplication makes signal errors dependent, so more diverse data is needed to calculate the derivative
	typename State<T>::iterator iBuffer = pSource->msBuffer.begin();
	typename State<T>::iterator iOutput = pSource->msOutput.begin();
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iError = msError.begin();
	while(iError.inBounds()) {
		*iBuffer += (pConn->opderiv(iError)) * (*iSignal) / (*iOutput); 
		++iError;
		++iSignal;
		++iOutput;
		++iBuffer;
	}
	return msBuffer;
}

template<typename T>
State<T>& Neuron<T>::subtractD(Neuron<T>* pSource, Connection<T>* pConn) {	//use argument to step back?
	//see Neuron::addD for helpful code comments
	typename State<T>::iterator iBuffer = pSource->msBuffer.begin();
	typename State<T>::iterator iError = msError.begin();
	while(iError.inBounds()) {
		*iBuffer -= pConn->opderiv(iError);
		++iError;
		++iBuffer;
	}
	return pSource->msBuffer;
	return msBuffer;
}

template<typename T>
State<T>& Neuron<T>::tanh() { 
	//these activation functions are the reason why Benoit can currently only handle floating point arithmetic
	//the forward activation function is applied to Signal, and the result is stored in Output
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
	//see Neuron::tanh for helpful code comments
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iOutput = msOutput.begin();
	while(iSignal.inBounds()) {
		*iOutput = *iSignal;		//no template type problems with linear function
		++iOutput;
		++iSignal;
	}
	return msOutput;
}

template<typename T>
State<T>& Neuron<T>::rbf() { 
	//see Neuron::tanh for helpful code comments
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iOutput = msOutput.begin();
	while(iSignal.inBounds()) {
		iOutput = exp(-((*iSignal)^2.0)); 	//exp doesn't take templates?
		++iOutput;
		++iSignal;
	}
	return msOutput;
}

template<typename T>
State<T>& Neuron<T>::tanhD() { //use argument to step back?
	//the derivative of the activation function is called and the result is multiplied by
	//the existing error gradient as per the chain rule
	//the result is stored in Error
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
	//see Neuron::tanhD for helpful code comments
	typename State<T>::iterator iError = msError.begin();
	typename State<T>::iterator iBuffer = msBuffer.begin();
	while(iError.inBounds()) {
		*iError = *iBuffer;
	}
	return msError;
}

template<typename T>
State<T>& Neuron<T>::rbfD() { //use argument to step back?
	//see Neuron::tanhD for helpful code comments
	typename State<T>::iterator iSignal = msSignal.begin();
	typename State<T>::iterator iError = msError.begin();
	typename State<T>::iterator iBuffer = msBuffer.begin();
	while(iError.inBounds()) {
		*iError = (*iBuffer) * -2.0*(*iSignal)*exp(-((*iSignal)^2.0)); //exp + templates?
	}
	return msError;
}

template<typename T> 
Neuron<T>::Neuron(	const unsigned int nSamples,
					const unsigned int nTimeSteps,
					const T tBias,
					const bool bTrainable) 
	: Neuron_base(bTrainable), mtBias(tBias), 
		msSignal(nSamples, nTimeSteps), msOutput(nSamples, nTimeSteps),
		msError(nSamples), msBuffer(nSamples) {
}

template<typename T>
Neuron<T>::Neuron(const Neuron<T>& cSource) 
	: Neuron_base(cSource), mtBias(cSource.mtBias),
		msSignal(cSource.msSignal.samples(),cSource.msSignal.steps()), 
		msOutput(cSource.msOutput.samples(),cSource.msOutput.steps()),
		msBuffer(cSource.msBuffer.samples()), 
		msError(cSource.msError.samples()) {
}

template<typename T>
Node& Neuron<T>::fire() { //not finished (step sequence)
	using namespace std;
	msSignal.initializeStep(mtBias);	//initialize each current sample in Signal with the bias
	deque<Connection_base*>::iterator it = mqConnIn.begin();
	deque<Connection_base*>::iterator ite = mqConnIn.end();
	while(it!=ite) {
		op(*it);	//call operator, which writes new data to Signal
		(*it)->mbSignalFlag = false;	//mark signal as received
		++it;
	}
	
	activationfcn();	//calls activation function on Signal and puts the result in Output
	
	it = mqConnOut.begin();
	ite = mqConnOut.end();
	while(it!=ite) {
		(*it)->mbSignalFlag = true;		//mark outputs as sent
		++it;
	}
	return *this;
}

template<typename T>
Node& Neuron<T>::backPropagate(const unsigned int nStepsBack) { //not finished (step sequence)
	//backPropagation relies, more than fire, on interoperatibility of Neuron and Connection
	using namespace std;
	msBuffer.initializeStep(0.0);		//set all current samples in Buffer to zero
	deque<Connection_base*>::iterator it = mqConnOut.begin();
	deque<Connection_base*>::iterator ite = mqConnOut.end();
	Neuron<T>* pTarget = NULL;
	Connection<T>* pConn = NULL;
	while(it!=ite) {
		//getTarget will return a Neuron_base*, but it's guaranteed to be a Neuron by other methods
		pTarget = static_cast<Neuron<T>*>((*it)->getTarget()); 
		pConn = static_cast<Connection<T>*>(*it);	//other methods guarantee all Connection_base* are Connection*
		pTarget->opderiv(this,pConn);		//call the output Neuron's operator derivative
		(*it)->mbErrorFlag = false;			//mark error as received
		++it;
	}
	
	activationfcnderiv();		//calls derivative of activation function and puts the result in Error
	
	it = mqConnIn.begin();
	ite = mqConnIn.end();
	while(it!=ite) {
		(*it)->mbErrorFlag = true;	//mark errors as sent
		++it;
	}
	return *this;
}

template<typename T>
Node& Neuron<T>::addInput(	const Node* pNewIn,
							const char chOperator,
							const unsigned int nTimeDelay) {
	if(pNewIn->mbGroup) pNewIn->addOutput(this, chOperator, nTimeDelay); //allow NodeGroup to create needed objects
	else new Connection<T>(pNewIn, this, chOperator, nTimeDelay);	//create a single Connection (not Connection_base)
	return *this;
}

template<typename T>
Node& Neuron<T>::addOutput(	const Node* pNewOut,
							const char chOperator,
						  	const unsigned int nTimeDelay) {
	if(pNewOut->mbGroup) pNewOut->addInput(this, chOperator, nTimeDelay); //allow NodeGroup to create needed objects
	else new Connection<T>(this, pNewOut, chOperator, nTimeDelay);	//create a single Connection (not Connection_base)
	return *this;
}

template<typename T>
ostream& Neuron<T>::print(ostream &out) const {
	//weight, call Neuron_base::print
	//state data?
	return out;
}

#endif

