#ifndef NATIVEERRORSQUARE_H
#define NATIVEERRORSQUARE_H

#include <vector>
#include <IObjectiveFunction.h>

CALIMERO_DECLARE_OFUNCTION(NativeErrorSquare)
public:
    NativeErrorSquare();
    virtual ~NativeErrorSquare();
    std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                 std::vector<Variable*> observedparameters,
                                 std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
};

CALIMERO_DECLARE_OFUNCTION(SSE)
public:
    SSE();
    std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                 std::vector<Variable*> observedparameters,
                                 std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
};

CALIMERO_DECLARE_OFUNCTION(MSE)
public:
    MSE();
    std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                 std::vector<Variable*> observedparameters,
                                 std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
};

#endif // EXTERNALMODEL_H
