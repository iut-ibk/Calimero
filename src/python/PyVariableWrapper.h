#ifndef PYVARIABLEWRAPPER_H
#define PYVARIABLEWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <Domain.h>

using namespace boost::python;

void wrapVariable()
{
    enum_<VARTYPE>("VARTYPE")
        .value("CALIBRATIONVARIABLE", CALIBRATIONVARIABLE)
        .value("OBJECTIVEFUNCTIONVARIABLE", OBJECTIVEFUNCTIONVARIABLE)
        .value("ITERATIONVARIABLE", ITERATIONVARIABLE)
        .value("OBSERVEDVARIABLE", OBSERVEDVARIABLE)
        ;

    class_<Variable >("Variable",init<std::string, vector<double>, VARTYPE>())
        .def("getValues", &Variable::getValues)
        .def("setValues", &Variable::setValues)
        .def("getName", &Variable::getName)
        .def("getType", &Variable::getType)
        ;

    class_<CalibrationVariable, bases<Variable> >("CalibrationVariable", init<std::string, vector<double> >())
        .def("getInitValues", &CalibrationVariable::getInitValues)
        .def("setInitValues", &CalibrationVariable::setInitValues)
        .add_property("max", &CalibrationVariable::getMax, &CalibrationVariable::setMax)
        .add_property("min", &CalibrationVariable::getMin, &CalibrationVariable::setMin)
        ;

    class_<ObjectiveFunctionVariable, bases<Variable> >("ObjectiveFunctionVariable", init<std::string>())
        .def("addParameter", &ObjectiveFunctionVariable::addParameter)
        .def("removeParameter", &ObjectiveFunctionVariable::removeParameter)
        .def("getObjectiveFunction", &ObjectiveFunctionVariable::getObjectiveFunction)
        .def("setObjectiveFunction", &ObjectiveFunctionVariable::setObjectiveFunction)
        ;
}




#endif // PYVARIABLEWRAPPER_H
