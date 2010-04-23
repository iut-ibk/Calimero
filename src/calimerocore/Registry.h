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

template <typename T> class Registry;

template <typename T> class Registry
{
    typedef void (*regProto) (Registry<T> *reg);
    typedef map<string, IFunctionFactory*> factorymap;
    private:
        IFUNCTIONTYPE type;
        factorymap registered_factories;

    public:
        Registry();
        ~Registry();
        IFUNCTIONTYPE getType();
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

template <typename T> Registry<T>::~Registry(){};

template <typename T> bool Registry<T>::registerFunction(IFunctionFactory* factory)
{
    if(!contains(factory->getFunctionName()))
    {
        Logger(Debug) << factory->getFunctionName() << " not registered yet";
        registered_factories[factory->getFunctionName()]=factory;
        return true;
    }
    else
    {
        Logger(Debug) << factory->getFunctionName() << " already registered";
        return false;
    }
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
    std::pair<string, IFunctionFactory*> p;
    BOOST_FOREACH(p,registered_factories)
            Logger(Debug) << p.first;

    return registered_factories.find(name)!=registered_factories.end();
}

template <typename T> bool Registry<T>::addNativePlugin(const std::string &plugin_path) {
        QLibrary l(QString::fromStdString(plugin_path));
        bool loaded = l.load();
        if(!loaded)
        {
            Logger(Error) << "could not load plugin: " << plugin_path;
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
        case NOTYPE:
            Logger(Error) << "Registry has no valid type";
            return false;
            break;
        }

        if (regFunction) {
                regFunction(this);
        } else {
                Logger(Warning) << plugin_path << " has no function register hook";
                return false;
        }
        return true;
}

template <typename T> IFUNCTIONTYPE Registry<T>::getType()
{
    return type;
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
