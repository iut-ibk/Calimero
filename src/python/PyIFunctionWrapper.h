#ifndef PYIFUNCTIONWRAPPER_H
#define PYIFUNCTIONWRAPPER_H

#include <IFunction.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <map>
#include <string>

using namespace std;


void wrapIFunction()
{
    enum_<DATATYPE>("DATATYPE")
            .value("STRING", STRING)
            .value("DOUBLE", DOUBLE)
            ;

    class_<std::map<std::string, DATATYPE> >("datatypmap")
            .def(map_indexing_suite<std::map<std::string, DATATYPE> >())
            ;

    class_<IFunction, boost::noncopyable>("IFunction")
            .def("containsParameter", &IFunction::containsParameter)
            .def("setValueOfParameter", &IFunction::setValueOfParameter)
            .def("getDataTypes", &IFunction::getDataTypes)
            .def("getValueOfParameter", &IFunction::getValueOfParameter)
            .def("setDataType", &IFunction::setDataType)
            ;

}

#endif // PYIFUNCTIONWRAPPER_H
