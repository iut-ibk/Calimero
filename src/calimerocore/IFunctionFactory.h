#ifndef IFUNCTIONFACTORY_H
#define IFUNCTIONFACTORY_H

#include <string>
#include <CalimeroGlob.h>
#include <IFunction.h>
#include <Logger.h>

class IFunctionFactory {
    public:
        virtual ~IFunctionFactory(){}
        virtual IFunction *createFunction() const = 0;
        virtual std::string getFunctionName() = 0;
};

template <typename T> class NativeFunctionFactory : public IFunctionFactory
{
    public:
        NativeFunctionFactory();
        virtual T *createFunction() const;
        virtual std::string getFunctionName();
};

template <typename T> NativeFunctionFactory<T>::NativeFunctionFactory() {}

template <typename T> T *NativeFunctionFactory<T>::createFunction() const
{
    Logger(Debug) << "createFunction in NativeFunctionFactory called";
    return new T();
}

template <typename T> std::string NativeFunctionFactory<T>::getFunctionName()
{
        return T::name;
}

#endif // IFUNCTIONFACTORY_H
