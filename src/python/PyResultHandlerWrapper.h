#ifndef PYRESULTHANDLERWRAPPER_H
#define RESULTHANDLERWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <iostream>
#include <PyEnv.h>
#include <IResultHandler.h>
#include <boost/shared_ptr.hpp>

using namespace boost::python;
using namespace boost;
using namespace std;


struct ResultHandlerWrapper : public IResultHandler, wrapper<IResultHandler> {
    ResultHandlerWrapper() {

    }

    virtual ~ResultHandlerWrapper() {

    }

    bool run(std::vector<shared_ptr<IterationResult> > iterationresults)
    {
        ScopedGILRelease scoped;
        try {
                if (python::override f = this->get_override("run"))
                    return f(iterationresults);
                else
                        throw CalimeroException("No methode with name \"run\" found");
        }
        catch(python::error_already_set const &)
        {
                handle_python_exception("Error in run methode of a result handler");
                return false;
        }

        return false;
    }

    object self;
};

void wrapResultHandlerWrapper()
{
    python::implicitly_convertible<auto_ptr<ResultHandlerWrapper>, auto_ptr<IResultHandler> >();
    class_<ResultHandlerWrapper, bases<IFunction>, auto_ptr<ResultHandlerWrapper>, boost::noncopyable>("IResultHandler")
            .def("run", pure_virtual(&ResultHandlerWrapper::run))
            .def("test", &IResultHandler::test)
            ;
}
#endif // PYRESULTHANDLERWRAPPER_H
