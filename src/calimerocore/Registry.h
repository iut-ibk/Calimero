#ifndef OFUNCTIONREG_H
#define OFUNCTIONREG_H

#include <CalimeroGlob.h>
#include <map>
#include <string>
#include <list>
#include <IFunctionFactory.h>
#include <Logger.h>
#include <QLibrary>
#include <QString>
#include <IFunction.h>
#include <boost/noncopyable.hpp>

using namespace std;

class IFunctionFactory;

template <typename T> class Registry;

template <typename T> class Registry : private boost::noncopyable
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
        void addNativePlugin(const std::string &plugin_path);
        T* getFunction(string name);
        bool contains(string name);
};

template <typename T> Registry<T>::Registry()
{
    type = T::getType();
    Logger(Error) << "Registry instance of type: " << type;

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

template <typename T> bool Registry<T>::contains(string name)
{
    std::pair<string, IFunctionFactory*> p;
    BOOST_FOREACH(p,registered_factories)
            Logger(Debug) << p.first;

    return registered_factories.find(name)!=registered_factories.end();
}

template <typename T> void Registry<T>::addNativePlugin(const std::string &plugin_path) {
        QLibrary l(QString::fromStdString(plugin_path));
        bool loaded = l.load();
        if(!loaded)
        {
            Logger(Error) << "could not load plugin: " << plugin_path;
            return;
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
            break;
        }

        if (regFunction) {
                regFunction(this);
        } else {
                Logger(Warning) << plugin_path << " has no function register hook";
        }
}

template <typename T> IFUNCTIONTYPE Registry<T>::getType()
{
    return type;
};

#endif // OFUNCTIONREG_H
