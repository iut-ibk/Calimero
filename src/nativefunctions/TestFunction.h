#ifndef TESTNODE_H
#define TESTNODE_H

#include <IObjectiveFunction.h>

CALIMERO_DECLARE_OFUNCTION(TestOFunction)
public:
        TestOFunction();
        virtual ~TestOFunction();
        std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                         std::vector<Variable*> observedparameters,
                                         std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
};

#endif // TESTNODE_H
