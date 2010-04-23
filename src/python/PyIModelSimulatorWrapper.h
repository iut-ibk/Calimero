#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <IObjectiveFunction.h>
#include <iostream>
#include <ObjectiveFunctionVariable.h>
#include <Variable.h>
#include <IModelSimulator.h>
#include <PyEnv.h>


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

    bool exec(Domain *dom)
    {
        try {
                if (python::override f = this->get_override("exec")) {
                        return f(dom);
                } else {
                        //TODO do something here now reason there is no f method
                }
        } catch(python::error_already_set const &) {
                Logger(Error) << __FILE__ << ":" << __LINE__;
                handle_python_exception();
        }
        return false;
    }

    void stop()
    {
        try {
                if (python::override f = this->get_override("stop")) {
                        f();
                } else {
                        //TODO do something here now reason there is no f method
                }
        } catch(python::error_already_set const &) {
                Logger(Error) << __FILE__ << ":" << __LINE__;
                handle_python_exception();
        }
    }


private:
    PyObject *self;
};

void wrapIModelSimulator()
{
        python::implicitly_convertible<auto_ptr<IModelSimulatorWrapper>, auto_ptr<IModelSimulator> >();
        class_<IModelSimulator, bases<IFunction>, auto_ptr<IModelSimulatorWrapper>, boost::noncopyable>("IModelSimulator")
                .def("exec", pure_virtual(&IModelSimulatorWrapper::exec))
                ;
}
