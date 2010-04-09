#ifndef PYVARIABLEWRAPPER_H
#define PYVARIABLEWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>

using namespace boost::python;

void wrapVariable()
{
        enum_<Variable::VARTYPE>("VARTYPE")
                .value("CALIBRATIONVARIABLE", Variable::CALIBRATIONVARIABLE)
                .value("OBJECTIVEFUNCTIONVARIABLE", Variable::OBJECTIVEFUNCTIONVARIABLE)
                .value("ITERATIONVARIABLE", Variable::ITERATIONVARIABLE)
                .value("OBSERVEDVARIABLE", Variable::OBSERVEDVARIABLE)
                ;

        class_<Variable>("Variable",init<std::string, vector<double>, Variable::VARTYPE>())
                .def("getType",&Variable::getType)
                .add_property("name", &Variable::getName, &Variable::setName)
                .def("getValues", &Variable::getValues)
                .def("setValues", &Variable::setValues)
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
