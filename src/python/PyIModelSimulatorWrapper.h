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
                ScopedGILRelease scope;
                Logger(Debug) << "exec IModelSimulator";
                if (python::override f = this->get_override("execModel"))
                {
                    bool result = f(ptr(dom));
                    Logger(Debug) << "exec IModelSimulator DONE";
                    return result;
                }
                else
                        throw CalimeroException("No methode with name \"execModel\" found");
            }
            catch(python::error_already_set const &)
            {
                    handle_python_exception("Error in exec method of a model simulator");
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
                    handle_python_exception("Error in stop method of a model simulator");
            }
    }

    object self;
};

void wrapIModelSimulator()
{
        python::implicitly_convertible<auto_ptr<IModelSimulatorWrapper>, auto_ptr<IModelSimulator> >();
        class_<IModelSimulatorWrapper, bases<IFunction>, auto_ptr<IModelSimulatorWrapper>, boost::noncopyable>("IModelSimulator")
                .def("execModel", pure_virtual(&IModelSimulatorWrapper::exec))
                ;
}
