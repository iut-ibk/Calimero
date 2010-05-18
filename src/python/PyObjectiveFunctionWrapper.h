#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <IObjectiveFunction.h>
#include <iostream>
#include <ObjectiveFunctionVariable.h>
#include <Variable.h>
#include <PyEnv.h>
#include <Exception.h>

using namespace boost::python;
using namespace std;


struct ObjectiveFunctionInterfaceWrapper : public IObjectiveFunction, wrapper<IObjectiveFunction> {
    ObjectiveFunctionInterfaceWrapper() {

    }

    virtual ~ObjectiveFunctionInterfaceWrapper() {

    }

    std::vector<double> eval(std::vector<Variable*> iterationparameters,
                             std::vector<Variable*> observedparameters,
                             std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
    {
        try {
                if (python::override f = this->get_override("eval"))
                    return f(iterationparameters, observedparameters, objectivefunctionparameters);
                else
                   throw CalimeroException("No methode with name \"eval\" found");

        }
        catch(python::error_already_set const &exception)
        {
            handle_python_exception("Error in eval methode of an objective function");
        }

        return vector<double>(0);
    }

    object self;
};

void wrapOFunction()
{
        python::implicitly_convertible<auto_ptr<ObjectiveFunctionInterfaceWrapper>, auto_ptr<IObjectiveFunction> >();
        class_<ObjectiveFunctionInterfaceWrapper, bases<IFunction>, auto_ptr<ObjectiveFunctionInterfaceWrapper>, boost::noncopyable>("IObjectiveFunction")
                .def("eval", pure_virtual(&ObjectiveFunctionInterfaceWrapper::eval))
                ;
}
