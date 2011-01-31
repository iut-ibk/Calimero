#ifndef TESTNODE_H
#define TESTNODE_H

#include <IObjectiveFunction.h>

CALIMERO_DECLARE_OFUNCTION(DummyOFunction)
public:
        DummyOFunction();
        virtual ~DummyOFunction();
        int fac(int x);
        std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                         std::vector<Variable*> observedparameters,
                                         std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
};

#endif // TESTNODE_H
