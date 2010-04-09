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
    protected:
        map<string,DATATYPE> parametertypes;
        map<string,string> parametervalues;

    public:
        static IFUNCTIONTYPE getType()
        {
            return OBJECTIVEFUNCTION;
        };

        IObjectiveFunction(){};

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

        virtual std::vector<double> eval(std::vector<Variable*>* iterationparameters,
                                         std::vector<Variable*>* observedparameters,
                                         std::vector<ObjectiveFunctionVariable*>* objectivefunctionparameters) = 0;
};
#endif // OBJECTIVEFUNCTIONINTERFACE_H_INCLUDED
