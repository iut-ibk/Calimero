#ifndef IFUNCTION_H
#define IFUNCTION_H

#include <string>
#include <map>
#include <Logger.h>
#include <boost/foreach.hpp>

enum IFUNCTIONTYPE { NOTYPE = -1,
                     OBJECTIVEFUNCTION = 0,
                     CALIBRATIONALGORITHM = 1,
                     MODELSIMULATOR = 2
                    };

enum DATATYPE { STRING = 1,
                DOUBLE = 2
              };

using namespace std;
using namespace boost;

class IFunction
{
protected:
    map<string,DATATYPE> parametertypes;
    map<string,string> parametervalues;
public:

    static IFUNCTIONTYPE getType()
    {
        return NOTYPE;
    }

    map<string, DATATYPE > getDataTypes()
    {
        return parametertypes;
    }

    bool setDataType(string name, DATATYPE type)
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

    bool containsParameter(string name)
    {
        return parametertypes.find(name)!=parametertypes.end();
    }

    string getValueOfParameter(string name)
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

    bool setValueOfParameter(string name, string value)
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
};

#endif // IFUNCTION_H
