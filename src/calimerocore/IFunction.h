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
public:

    static IFUNCTIONTYPE getType()
    {
        return NOTYPE;
    }

    virtual map<string, DATATYPE > getDataTypes() = 0;
    virtual bool setDataType(string name, DATATYPE type) = 0;
    virtual bool containsParameter(string name) = 0;
    virtual string getValueOfParameter(string name) = 0;
    virtual bool setValueOfParameter(string name, string value) = 0;
};

#endif // IFUNCTION_H
