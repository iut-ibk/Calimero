#ifndef OBJECTIVEFUNCTIONINTERFACE_H_INCLUDED
#define OBJECTIVEFUNCTIONINTERFACE_H_INCLUDED

#include <vector>
#include <CalimeroGlob.h>
#include <IFunction.h>

#define CALIMERO_DECLARE_OFUNCTION(function)  \
class CALIMERO_PUBLIC function : public IObjectiveFunction { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_OFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class Variable;
class ObjectiveFunctionVariable;

class CALIMERO_PUBLIC IObjectiveFunction : public IFunction
{
    public:

        IObjectiveFunction(){};
        virtual ~IObjectiveFunction() {}

        static IFUNCTIONTYPE getType()
        {
            return OBJECTIVEFUNCTION;
        }

        virtual std::vector<double> eval(std::vector<Variable*> iterationparameters,
                                         std::vector<Variable*> observedparameters,
                                         std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters) = 0;
};
#endif // OBJECTIVEFUNCTIONINTERFACE_H_INCLUDED
