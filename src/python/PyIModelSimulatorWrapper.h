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
    IModelSimulatorWrapper()
    {
    }

    virtual ~IModelSimulatorWrapper()
    {
    }

    bool exec(Domain *dom)
    {
        try {
                if (python::override f = this->get_override("exec"))
                        return f(ptr(dom));
                else
                        throw CalimeroException("No methode with name \"exec\" found");
            }
            catch(python::error_already_set const &)
            {
                    handle_python_exception("Error in exec methode of a model simulator");
            }

        return false;
    }

    void stop()
    {
        try {
                if (python::override f = this->get_override("stop"))
                        f();
                else
                    throw CalimeroException("No methode with name \"start\" found");
            }
            catch(python::error_already_set const &)
            {
                    handle_python_exception("Error in start methode of a calibration algorithm");
            }
    }

    object self;
};

void wrapIModelSimulator()
{
        python::implicitly_convertible<auto_ptr<IModelSimulatorWrapper>, auto_ptr<IModelSimulator> >();
        class_<IModelSimulatorWrapper, bases<IFunction>, auto_ptr<IModelSimulatorWrapper>, boost::noncopyable>("IModelSimulator")
                .def("exec", pure_virtual(&IModelSimulatorWrapper::exec))
                ;
}
