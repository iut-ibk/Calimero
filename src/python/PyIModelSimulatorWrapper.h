#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <IObjectiveFunction.h>
#include <iostream>
#include <ObjectiveFunctionVariable.h>
#include <Variable.h>
#include <IModelSimulator.h>


using namespace boost::python;
using namespace std;


struct IModelSimulatorWrapper : IModelSimulator, wrapper<IModelSimulator> {
    IModelSimulatorWrapper(PyObject *self)
    {
        this->self = self;
        Py_INCREF(self);
    }

    virtual ~IModelSimulatorWrapper()
    {
        Py_DECREF(self);
    }

    bool exec(vector<CalibrationVariable*> calibrationparameters,
                      vector<Variable*> observedparameters,
                      vector<Variable*> iterationparameters,
                      vector<ObjectiveFunctionVariable*> objectivefunctionparameters, IterationResult *result)
    {
        return call_method<bool>(self, "exec", iterationparameters, observedparameters, objectivefunctionparameters, result);
    }

    void stop()
    {
        return call_method<void>(self, "stop");
    }


private:
    PyObject *self;
};

void wrapIModelSimulator()
{
        class_<IModelSimulator, bases<IFunction>, auto_ptr<IModelSimulatorWrapper>, boost::noncopyable>("IModelSimulator")
                .def("exec", pure_virtual(&IModelSimulatorWrapper::exec))
                ;
}
