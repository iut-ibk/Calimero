#include <boost/format.hpp>
#include <boost/python.hpp>
#include <string>
#include <Registry.h>
#include <PyObjectiveFunctionWrapper.h>
#include <PyCalibrationAlgWrapper.h>
#include <PyVariableWrapper.h>
#include <PyIModelSimulatorWrapper.h>
#include <PyCalibrationEnvWrapper.h>
#include <PyIterationResultWrapper.h>
#include <PyRegistryWrapper.h>
#include <PyFunctionFactory.cpp>
#include <PyCalibrationWrapper.h>
#include <PyDomainWrapper.h>
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
#include <boost/python/scope.hpp>
#include <IterationResult.h>
#include <Exception.h>
#include <PyResultHandlerWrapper.h>

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

        class_<std::vector<string> >("stringvector")
                .def(vector_indexing_suite<std::vector<string> >());

        class_<std::vector<ObjectiveFunctionVariable*> >("objectivefunctionvariablevector")
                .def(vector_indexing_suite<std::vector<ObjectiveFunctionVariable*> >());

        class_<std::vector<Variable*> >("variablevector")
                .def(vector_indexing_suite<std::vector<Variable*> >());
        
        class_<std::vector<IterationResult*> >("iterationresultvector")
                .def(vector_indexing_suite<std::vector<IterationResult*> >());

        class_<std::vector<CalibrationVariable*> >("calibrationvariablevector")
                .def(vector_indexing_suite<std::vector<CalibrationVariable*> >());

        class_<std::map<string,string> >("stringmap")
                .def(map_indexing_suite<std::map<string,string> >());

        class_<std::map<int,DATATYPE> >("datatypemap")
                .def(map_indexing_suite<std::map<int, DATATYPE> >());

        class_<std::map<std::string, DATATYPE> >("datatypmap")
                .def(map_indexing_suite<std::map<std::string, DATATYPE> >());

        enum_<DATATYPE>("DATATYPE")
                .value("STRING", STRING)
                .value("DOUBLE", DOUBLE)
                .value("BOOL", BOOL)
                .value("INT", INT)
                .value("UINT", UINT)
                .value("FILESTRING", FILESTRING)
                .value("DIRSTRING", DIRSTRING)
                ;

        class_<IFunction, boost::noncopyable>("IFunction")
                .def("containsParameter", &IFunction::containsParameter)
                .def("setValueOfParameter", &IFunction::setValueOfParameter)
                .def("getDataTypes", &IFunction::getDataTypes)
                .def("getValueOfParameter", &IFunction::getValueOfParameter)
                .def("setDataType", &IFunction::setDataType)
                .def("getParameterValues", &IFunction::getParameterValues)
                ;

        wrapDomain();
        wrapCalibrationEnv();
        wrapIterationResult();
        wrapIModelSimulator();
        wrapOFunction();
        wrapVariable();
        wrapCalAlgFunction();
        wrapResultHandlerWrapper();
        wrapCalibration();
        wrapRegistry();
        wrapPyEnv();

        register_ptr_to_python< Variable* >();
        register_ptr_to_python< ObjectiveFunctionVariable* >();
        register_ptr_to_python< CalibrationVariable* >();
        register_ptr_to_python< IterationResult* >();
        register_ptr_to_python< CalibrationEnv* >();
        register_ptr_to_python< Calibration* >();
        register_ptr_to_python< Domain* >();

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

        PyEval_InitThreads();
        
        PyThreadState *pts = PyGILState_GetThisThreadState();
        PyEval_ReleaseThread(pts);

        ScopedGILRelease scoped;
        priv->main_module = import("__main__");
        priv->main_namespace = priv->main_module.attr("__dict__");
}

PyEnv::~PyEnv() {
        if(!Py_IsInitialized())
            Py_Finalize();
        delete priv;
}

void PyEnv::destroy()
{
    if(instance != 0)
        delete instance;
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
        ScopedGILRelease scoped;
        exec(fmt.str().c_str(),priv->main_namespace,priv->main_namespace);
}

void PyEnv::registerFunctions(IRegistry *registry, const string &module)
{
    ScopedGILRelease scoped;
    boost::format fmt;

    if(loadedmodules.find(module)==loadedmodules.end())
    {
        fmt = boost::format("import sys\n"
                      "import pycalimero\n"
                      "import %1%\n"
                      "clss = pycalimero.%2%.__subclasses__()\n");
    }
    else
    {
        fmt = boost::format("import sys\n"
                      "import pycalimero\n"
                      "reload(%1%)\n"
                      "clss = pycalimero.%2%.__subclasses__()\n");
    }

    switch(registry->getType())
    {
    case OBJECTIVEFUNCTION:
        fmt % module % "IObjectiveFunction";
        break;
    case CALIBRATIONALGORITHM:
        fmt % module % "ICalibrationAlg";
        break;
    case RESULTFUNCTION:
        fmt % module % "IResultHandler";
        break;
    case MODELSIMULATOR:
        fmt % module % "IModelSimulator";
        break;
    case NOTYPE:
        Logger(Error) << __LINE__;
        abort();
        break;
    }

   try {
           exec(fmt.str().c_str(), priv->main_namespace, priv->main_namespace);
           object clss = priv->main_namespace["clss"];
           int numn = 0;

           for (int i = 0; i < len(clss); i++)
           {
               switch(registry->getType())
               {
               case OBJECTIVEFUNCTION:
                   registry->registerFunction(new PyFunctionFactory<IObjectiveFunction>(clss[i]));
                   break;
               case CALIBRATIONALGORITHM:
                   registry->registerFunction(new PyFunctionFactory<ICalibrationAlg>(clss[i]));
                   break;
               case MODELSIMULATOR:
                   registry->registerFunction(new PyFunctionFactory<IModelSimulator>(clss[i]));
                   break;
               case RESULTFUNCTION:
                   registry->registerFunction(new PyFunctionFactory<IResultHandler>(clss[i]));
                   break;
               default:
                   break;
               }

               numn++;
           }

           loadedmodules.insert(module);

   } catch(error_already_set const &) {
           handle_python_exception("Could not register python function");
   }
}

void handle_python_exception(const std::string &msg)
{
    PyObject* type, *value, *traceback;
    string error = " | | ";
    try
    {
        PyErr_Fetch(&type, &value, &traceback);
        python::handle<> ty(type), v(value), tr(traceback);
        python::str format("%s|%s|%s");
        python::object ret = format % python::make_tuple(ty, v, tr);
        error = python::extract<string>(ret);
    }
    catch(...) //is a little bit dirty but it catches all boost/python exceptions
    {
        throw PythonException(error, msg);
        return;
    }

    throw PythonException(error, msg);
}

void wrapPyEnv()
{
    class_<PyEnv>("PyEnv",no_init)
            .def("addPythonPath", &PyEnv::addPythonPath)
            .def("getInstance", &PyEnv::getInstance, return_value_policy<reference_existing_object>())
            .def("destroy", &PyEnv::destroy)
            .staticmethod("getInstance")
            .staticmethod("destroy")
            .def("registerFunctions", &PyEnv::registerFunctions)
            ;
}
