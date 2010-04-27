#ifndef PYIFUNCTIONWRAPPER_H
#define PYIFUNCTIONWRAPPER_H

#include <IFunction.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <map>
#include <string>

using namespace std;

struct FunctionWrapper : IFunction, wrapper<IFunction> {
    object self;
};

#endif // PYIFUNCTIONWRAPPER_H
