#ifndef ICALIBRATIONALG_H
#define ICALIBRATIONALG_H

#include <CalimeroGlob.h>
#include <IFunction.h>
#include <Logger.h>

#define CALIMERO_DECLARE_CALFUNCTION(function)  \
class CALIMERO_PUBLIC function : public ICalibrationAlg { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_CALFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class CALIMERO_PUBLIC ICalibrationAlg : public IFunction
{
    protected:
        map<string,DATATYPE> parametertypes;
        map<string,string> parametervalues;
    public:
        static IFUNCTIONTYPE getType()
        {
            return CALIBRATIONALGORITHM;
        };

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

        virtual ~ICalibrationAlg(){}
        virtual bool start() = 0;
};


#endif // ICALIBRATIONALG_H
