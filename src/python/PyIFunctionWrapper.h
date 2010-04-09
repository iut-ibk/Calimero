#ifndef PYIFUNCTIONWRAPPER_H
#define PYIFUNCTIONWRAPPER_H

#include <IFunction.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <map>

void wrapIFunction()
{
    class_<std::map<std::string,DATATYPE> >("datatypmap")
            .def(map_indexing_suite<std::map<std::string, DATATYPE> >());

    enum_<DATATYPE>("DATATYPE")
            .value("STRING", STRING)
            .value("DOUBLE", DOUBLE)
            ;

}

#endif // PYIFUNCTIONWRAPPER_H
