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
    protected:
        map<string,DATATYPE> parametertypes;
        map<string,string> parametervalues;
    public:
        static IFUNCTIONTYPE getType()
        {
            return MODELSIMULATOR;
        }

        virtual map<string, DATATYPE > getDataTypes()
        {
            return parametertypes;
        }

        virtual bool setDataType(string name, DATATYPE type)
        {
            if(containsParameter(name))
            {
                Logger(Error) << "Parameter [" << name << "] already exists in IFunction";
                return false;
            }

            parametertypes[name]=DOUBLE;

            std::pair<string,DATATYPE> p;
            BOOST_FOREACH(p, parametertypes)
                Logger(Debug) << p.first << " : " << p.second;

            return true;

        }

        virtual bool containsParameter(string name)
        {
            return parametertypes.find(name)!=parametertypes.end();
        }

        virtual string getValueOfParameter(string name)
        {
            if(!containsParameter(name))
            {
                Logger(Error) << "IFunction does not contain parameter: [" << name << "]";
                return "";
            }

            if(parametervalues.find(name)==parametervalues.end())
            {
                Logger(Error) << "Parameter: [" << name << "] not set yet in IFunction";
                return "";
            }

            return parametervalues[name];
        }

        virtual bool setValueOfParameter(string name, string value)
        {
            if(!containsParameter(name))
            {
                Logger(Error) << "IFunction does not contain parameter: [" << name << "]";
                return false;
            }

            switch(parametertypes[name])
            {
            case STRING:
                break;
            case DOUBLE:
                try
                {
                    boost::lexical_cast<double>(value);
                }
                catch (const std::exception&)
                {
                    Logger(Error) << "Wrong DATATYPE in IFunction";
                    return false;
                }
                break;
            }

            parametervalues[name]=value;
            return  true;
        }


        virtual bool exec(vector<CalibrationVariable*> calibrationparameters,
                          vector<Variable*> observedparameters,
                          vector<Variable*> iterationparameters,
                          vector<ObjectiveFunctionVariable*> objectivefunctionparameters, IterationResult *result) = 0;
};

#endif // IMODELSIMULATOR_H
