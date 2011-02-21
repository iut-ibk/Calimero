#ifndef IMODELSIMULATOR_H
#define IMODELSIMULATOR_H

#include <vector>
#include <IFunction.h>
#include <CalimeroGlob.h>
#include <Logger.h>

class CalibrationVariable;
class ObjectiveFunctionVariable;
class Variable;
class IterationResult;
class Domain;

using namespace std;

#define CALIMERO_DECLARE_MODELSIMULATOR(function)  \
class CALIMERO_PUBLIC function : public IModelSimulator { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_MODELSIMULATOR_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }


class CALIMERO_PUBLIC IModelSimulator : public IFunction
{
    public:
        IModelSimulator(){};
        virtual ~IModelSimulator(){};
        static IFUNCTIONTYPE getType()
        {
            return MODELSIMULATOR;
        }


        virtual bool exec(Domain *dom) = 0;
};

#endif // IMODELSIMULATOR_H
