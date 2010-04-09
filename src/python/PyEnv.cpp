#include <boost/format.hpp>
#include <boost/python.hpp>
#include <string>
#include <Registry.h>
#include <PyObjectiveFunctionWrapper.h>
#include <PyCalibrationAlgWrapper.h>
#include <PyVariableWrapper.h>
#include <PyIModelSimulatorWrapper.h>
#include <PyIterationResultWrapper.h>
#include <PyFunctionFactory.cpp>
#include <iostream>
#include <boost/foreach.hpp>
#include <list>
#include <PyEnv.h>
#include <iostream>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <Log.h>
#include <LogSink.h>
#include <Logger.h>
#include <PyIFunctionWrapper.h>

using namespace boost::python;

void init() {
        OStreamLogSink *sink = new OStreamLogSink(cout);
        Log::init(sink, Debug);
}

void logdebug(std::string msg) {
        Logger(Debug) << msg;
}

void logwithlevel(std::string msg, LogLevel logl) {
        Logger(logl) << msg;
}

BOOST_PYTHON_MODULE(pycalimero)
{
        docstring_options doc_options;
        doc_options.disable_cpp_signatures();
        scope().attr("__doc__") = "calimero core lib for python.";

        class_<std::vector<double> >("doublevector")
                .def(vector_indexing_suite<std::vector<double> >());

        class_<std::vector<ObjectiveFunctionVariable*> >("objectivefunctionvariablevector")
                .def(vector_indexing_suite<std::vector<ObjectiveFunctionVariable*> >());

        class_<std::vector<Variable*> >("variablevector")
                .def(vector_indexing_suite<std::vector<Variable*> >());
        wrapIFunction();
        wrapIterationResult();
        wrapIModelSimulator();
        wrapOFunction();
        wrapVariable();
        wrapCalAlgFunction();
        def("init", ::init, "must be called first\n initializes the logger");
        def("log", logdebug);
        def("log", logwithlevel);
        enum_<LogLevel>("LogLevel")
                .value("debug", Debug)
                .value("standard", Standard)
                .value("error", Error)
                .value("warning", Warning)
                ;
}

struct PyEnvPriv {
        object main_module, main_namespace;
};

PyEnv *PyEnv::instance = 0;

PyEnv::PyEnv() {
        priv = new PyEnvPriv();
        if(!Py_IsInitialized())
            Py_Initialize();
        priv->main_module = import("__main__");
        priv->main_namespace = priv->main_module.attr("__dict__");
}

PyEnv::~PyEnv() {
        delete priv;
}

PyEnv *PyEnv::getInstance() {
        if (instance == 0) {
                instance = new PyEnv();
        }

        return instance;
}

void PyEnv::addPythonPath(std::string path) {
        boost::format fmt("import sys\n"
                          "sys.path.append('%1%')\n");
        fmt % path;
        exec(fmt.str().c_str(),priv->main_namespace,priv->main_namespace);
}

void PyEnv::registerFunctions(Registry<IObjectiveFunction> *registry, const string &module)
{
    boost::format fmt("import sys\n"
                      "import pycalimero\n"
                      "import %1%\n"
                      "clss = pycalimero.IObjectiveFunction.__subclasses__()\n");

    fmt % module;

   try {
           exec(fmt.str().c_str(), priv->main_namespace, priv->main_namespace);
           object clss = priv->main_namespace["clss"];
           int numn = 0;

           for (int i = 0; i < len(clss); i++)
           {
                   registry->registerFunction(new PyFunctionFactory<IObjectiveFunction>(clss[i]));
                   numn++;
           }

   } catch(error_already_set const &) {
           PyErr_Print();
           abort();
   }
}

void PyEnv::registerFunctions(Registry<ICalibrationAlg> *registry, const string &module)
{
    boost::format fmt("import sys\n"
                      "import pycalimero\n"
                      "import %1%\n"
                      "clss = pycalimero.ICalibrationAlg.__subclasses__()\n");

   fmt % module;

   try {
           exec(fmt.str().c_str(), priv->main_namespace, priv->main_namespace);
           object clss = priv->main_namespace["clss"];
           int numn = 0;

           for (int i = 0; i < len(clss); i++)
           {
                   registry->registerFunction(new PyFunctionFactory<ICalibrationAlg>(clss[i]));
                   numn++;
           }

   } catch(error_already_set const &) {
           PyErr_Print();
           abort();
   }
}
