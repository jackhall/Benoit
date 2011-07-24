#ifndef Bundle_h
#define Bundle_h

#include <vector>

class Bundle : private Connection {
private:
	//IOUNIT virtual double[] operator>>(const double[],IOUnit) = 0; //overloaded input operator
	//NEURALELEMENT double mdSignalState, mdErrorState, mdWeight;
	//NEURALELEMENT bool mbTrainable;
	//NEURALELEMENT vector<NeuralElement*> mvcInput;
	//NEURALELEMENT vector<NeuralElement*> mvcOutput;
	//NEURALELEMENT double (*SignalOperand)(vector<double>);
	//NEURALELEMENT double getSignalState() { return mdSignalState; }
	//NEURALELEMENT double getErrorState() { return mdErrorState; }
	//NEURALELEMENT double getWeight() { return mdWeight; }
	//NEURALELEMENT bool isTrainable() { return mbTrainable; }
	//CONNECTION bool mbSignalFlag, mbErrorFlag, mbTimeDelay;
	
protected:
	vector<Synapse> mvcSynapses;

public:
	vector<double> getSignalState();
	vector<double> getErrorState();
	vector<double> getWeight();
	vector<bool> isTrainable();
	
}

#endif
