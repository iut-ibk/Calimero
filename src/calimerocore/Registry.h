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
#ifndef OFUNCTIONREG_H
#define OFUNCTIONREG_H

#include <CalimeroGlob.h>
#include <map>
#include <boost/foreach.hpp>
#include <string>
#include <list>
#include <IFunctionFactory.h>
#include <Logger.h>
#include <QLibrary>
#include <QString>
#include <IFunction.h>
#include <boost/noncopyable.hpp>
#include <vector>

using namespace std;

class IFunctionFactory;

class IRegistry{
protected:
    IFUNCTIONTYPE type;
public:
    IFUNCTIONTYPE getType()
    {
        return type;
    }

    virtual bool registerFunction(IFunctionFactory* factory) = 0;
    virtual bool addNativePlugin(const std::string &plugin_path) = 0;
    virtual IFunction* getFunction(string name) = 0;
    virtual map<string,DATATYPE> getSettingTypes(string name) = 0;
    virtual bool contains(string name) = 0;
    virtual vector<string> getAvailableFunctions() = 0;
};

template <typename T> class Registry;

template <typename T> class Registry : public IRegistry
{
    typedef void (*regProto) (Registry<T> *reg);
    typedef map<string, IFunctionFactory*> factorymap;
    private:
        factorymap registered_factories;

    public:
        Registry();
        virtual ~Registry();
        bool registerFunction(IFunctionFactory* factory);
        bool addNativePlugin(const std::string &plugin_path);
        T* getFunction(string name);
        map<string,DATATYPE> getSettingTypes(string name);
        bool contains(string name);
        vector<string> getAvailableFunctions();
};

template <typename T> Registry<T>::Registry()
{
    type = T::getType();

    if(type==NOTYPE)
    {
        Logger(Error) << "Not allowed creating a non typed instance of class Registry";
        abort();
    }
};

typedef std::pair<string, IFunctionFactory *> str_ff_pair;

template <typename T> Registry<T>::~Registry() {
    BOOST_FOREACH(str_ff_pair f, registered_factories) {
        delete f.second;
    }
    registered_factories.clear();
};

template <typename T> bool Registry<T>::registerFunction(IFunctionFactory* factory)
{
    if (contains(factory->getFunctionName()))
    {
        delete registered_factories[factory->getFunctionName()];
        registered_factories.erase(factory->getFunctionName());
        Logger(Standard) << factory->getFunctionName() << " now reloaded";
    }
    else
    {
        Logger(Standard) << factory->getFunctionName() << " now registered";
    }

    registered_factories[factory->getFunctionName()]=factory;


    return true;
}

template <typename T> T*  Registry<T>::getFunction(string name)
{
    Logger(Debug) << "Try to create a instance of [" << name << "]";
    if(contains(name))
        return static_cast<T*>(registered_factories[name]->createFunction());

    Logger(Error) << "[" << name << "] does not exist -> return null pointer";
    return 0;
}


template <typename T> map<string,DATATYPE> Registry<T>::getSettingTypes(string name)
{
    if(!contains(name))
        abort();

    T* fun = getFunction(name);
    map<string,DATATYPE> result = ((IFunction*)fun)->getDataTypes();
    delete fun;
    return result;
}



template <typename T> bool Registry<T>::contains(string name)
{
    return registered_factories.find(name)!=registered_factories.end();
}

template <typename T> bool Registry<T>::addNativePlugin(const std::string &plugin_path) {
        QLibrary l(QString::fromStdString(plugin_path));
        bool loaded = l.load();

        if(!loaded)
        {
            Logger(Error) << l.errorString();
            return false;
        }

        regProto regFunction = 0;
        switch(getType())
        {
        case OBJECTIVEFUNCTION:
            regFunction = (regProto) l.resolve("registerObjectiveFunctions");
            break;
        case CALIBRATIONALGORITHM:
            regFunction = (regProto) l.resolve("registerCalibrationAlgs");
            break;
        case MODELSIMULATOR:
            regFunction =  (regProto) l.resolve("registerModelSimulations");
            break;
        case RESULTFUNCTION:
            regFunction =  (regProto) l.resolve("registerResultHandlers");
            break;
        case NOTYPE:
            Logger(Error) << "Registry has no valid type";
            return false;
            break;
        }

        if (regFunction) {
                regFunction(this);
        } else {
                Logger(Debug) << plugin_path << " has no function register hook";
                return false;
        }
        return true;
}

template <typename T > vector<string> Registry<T>::getAvailableFunctions()
{
    vector<string> result;
    std::pair<string,IFunctionFactory*> p;
    BOOST_FOREACH(p, registered_factories)
        result.push_back(p.first);
    return result;
}

#endif // OFUNCTIONREG_H
