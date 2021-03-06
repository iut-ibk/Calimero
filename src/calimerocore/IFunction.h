/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef IFUNCTION_H
#define IFUNCTION_H

#include <string>
#include <map>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <QFile>
#include <QDir>

enum IFUNCTIONTYPE { NOTYPE = -1,
                     OBJECTIVEFUNCTION = 0,
                     CALIBRATIONALGORITHM = 1,
                     MODELSIMULATOR = 2,
                     RESULTFUNCTION = 3
                    };

enum DATATYPE { STRING = 1,
                DOUBLE = 2,
                INT = 3,
                BOOL = 4,
                UINT = 5,
                FILESTRING = 6,
                DIRSTRING = 7
              };

using namespace std;
using namespace boost;

class IFunction
{
protected:
    map<string,DATATYPE> parametertypes;
    map<string,string> parametervalues;
public:
    virtual ~IFunction() {}

    static IFUNCTIONTYPE getType()
    {
        return NOTYPE;
    }

    map<string, DATATYPE > getDataTypes()
    {
        return parametertypes;
    }


    map< string,string > getParameterValues()
    {
        return parametervalues;
    }

    bool setDataType(string name, DATATYPE type, string value)
    {
        if(containsParameter(name))
        {
            Logger(Error) << "Parameter [" << name << "] already exists in IFunction";
            return false;
        }

        parametertypes[name]=type;

        return setValueOfParameter(name,value);
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
        case BOOL:
            {
                try
                {
                    int test = boost::lexical_cast<int>(value);
                    if(test!=0 && test!=1)
                        return false;
                }
                catch (const std::exception&)
                {
                    Logger(Error) << "Wrong DATATYPE in IFunction";
                    return false;
                }
                break;
            }
        case UINT:
            {
                try
                {
                    boost::lexical_cast<uint>(value);
                }
                catch (const std::exception&)
                {
                    Logger(Error) << "Wrong DATATYPE in IFunction";
                    return false;
                }
                break;
            }
        case INT:
            {
                try
                {
                    boost::lexical_cast<int>(value);
                }
                catch (const std::exception&)
                {
                    Logger(Error) << "Wrong DATATYPE in IFunction";
                    return false;
                }
                break;
            }
        case STRING:
            break;
        case FILESTRING:
            {
                if(!QFile::exists(QString::fromStdString(value)) && value != "")
                {
                    Logger(Error) << "\"" << value << "\" does not exist";
                    return false;
                }
                break;
            }
        case DIRSTRING:
            {
                if(!QFile::exists(QString::fromStdString(value)) && value != "")
                {
                    Logger(Error) << "\"" << value << "\" does not exist";
                    return false;
                }
                break;
            }
        case DOUBLE:
            {
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
        }

        parametervalues[name]=value;
        return  true;
    }

    bool setValues(map<string,string> values)
    {
        std::pair<string,string>p;
        bool ok = true;
        BOOST_FOREACH(p, values)
                ok = ok && setValueOfParameter(p.first,p.second);

        return ok;
    }
};

#endif // IFUNCTION_H
