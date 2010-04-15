#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <IObjectiveFunction.h>
#include <iostream>
#include <ObjectiveFunctionVariable.h>
#include <Variable.h>

using namespace boost::python;
using namespace std;


struct ObjectiveFunctionInterfaceWrapper : public IObjectiveFunction, wrapper<IObjectiveFunction> {
    ObjectiveFunctionInterfaceWrapper(PyObject *self)
    {
        this->self = self;
        Py_INCREF(self);
    }

    virtual ~ObjectiveFunctionInterfaceWrapper()
    {
        Py_DECREF(self);
    }

    std::vector<double> eval(std::vector<Variable*>* iterationparameters,
                             std::vector<Variable*>* observedparameters,
                             std::vector<ObjectiveFunctionVariable*>* objectivefunctionparameters)
    {
        return call_method<std::vector<double> >(self, "eval", iterationparameters, observedparameters, objectivefunctionparameters);
    }

private:
    PyObject *self;
};

void wrapOFunction()
{
        class_<IObjectiveFunction, bases<IFunction>, auto_ptr<ObjectiveFunctionInterfaceWrapper>, boost::noncopyable>("IObjectiveFunction")
                .def("eval", pure_virtual(&ObjectiveFunctionInterfaceWrapper::eval))
                ;
}
