#include <boost/format.hpp>
#include <boost/python.hpp>
#include <string>
#include <Registry.h>
/*#include <PyObjectiveFunctionWrapper.h>
#include <PyCalibrationAlgWrapper.h>
#include <PyVariableWrapper.h>
#include <PyIModelSimulatorWrapper.h>
#include <PyCalibrationEnvWrapper.h>
#include <PyIterationResultWrapper.h>
#include <PyRegistryWrapper.h>
#include <PyFunctionFactory.cpp>
#include <PyCalibrationWrapper.h>
#include <PyDomainWrapper.h>
#include <PyResultHandlerWrapper.h>*/
#include <Calibration.h>
#include <CalibrationEnv.h>
#include <iostream>
#include <boost/foreach.hpp>
#include <list>
#include <PyEnv.h>
#include <iostream>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <Log.h>
#include <LogSink.h>
#include <Logger.h>
#include <boost/python/scope.hpp>
#include <IterationResult.h>
#include <Exception.h>

#include <swigruntime.h>

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

bool execIteration(vector<CalibrationVariable*> calibrationparameters)
{
    bool result;
    Py_BEGIN_ALLOW_THREADS
    result = CalibrationEnv::getInstance()->execIteration(calibrationparameters);
    Py_END_ALLOW_THREADS
    return result;
}

void barrier()
{
    Py_BEGIN_ALLOW_THREADS
    CalibrationEnv::getInstance()->barrier();
    Py_END_ALLOW_THREADS
};

/*BOOST_PYTHON_MODULE(pycalimero)
{
        docstring_options doc_options;
        doc_options.disable_cpp_signatures();
        scope().attr("__doc__") = "calimero core lib for python.";

        class_<std::vector<double> >("doublevector")
                .def(vector_indexing_suite<std::vector<double> >());

        class_<std::vector<vector<double> > >("doublevectorvector")
                .def(vector_indexing_suite<std::vector<vector<double> > >());

        class_<std::vector<string> >("stringvector")
                .def(vector_indexing_suite<std::vector<string> >());

        class_<std::vector<ObjectiveFunctionVariable*> >("objectivefunctionvariablevector")
                .def(vector_indexing_suite<std::vector<ObjectiveFunctionVariable*> >());

        class_<std::vector<Variable*> >("variablevector")
                .def(vector_indexing_suite<std::vector<Variable*> >());

        class_<std::vector<shared_ptr<IterationResult> > >("iterationresultvector")
                .def(vector_indexing_suite<std::vector<shared_ptr<IterationResult> >, true >());

        class_<std::vector<CalibrationVariable*> >("calibrationvariablevector")
                .def(vector_indexing_suite<std::vector<CalibrationVariable*> >());

        class_<std::map<string,vector<double> > >("stringvectormap")
                .def(map_indexing_suite<std::map<string, vector<double> > >());

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

        def("barrier", barrier);
        def("execIteration", execIteration);
        register_ptr_to_python< Variable* >();
        register_ptr_to_python< ObjectiveFunctionVariable* >();
        register_ptr_to_python< CalibrationVariable* >();
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
}*/

struct PyEnvPriv {
        //object main_module, main_namespace;
        PyObject *main_namespace;
        //Py_O
};

PyEnv *PyEnv::instance = 0;
extern "C" {
void init_pycalimero();
}

PyEnv::PyEnv() {
        pythonmainthread=false;
        priv = new PyEnvPriv();

        if(!Py_IsInitialized()) {
            Py_Initialize();
            init_pycalimero();
        }

        //PyEval_InitThreads();

        PyObject *main = PyImport_ImportModule("__main__");
        priv->main_namespace = PyModule_GetDict(main);
        Py_DECREF(main);

        if (PyErr_Occurred()) {
            PyErr_Print();
        }

        /*PyThreadState *pts = PyGILState_GetThisThreadState();
        PyEval_ReleaseThread(pts);*/
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
        PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
        /*ScopedGILRelease scoped;
        exec(fmt.str().c_str(),priv->main_namespace,priv->main_namespace);*/
}

void PyEnv::registerFunctions(IRegistry *registry, const string &module)
{
    PyObject *pycalimero_module = PyImport_ImportModule("pycalimero");
    if (PyErr_Occurred()) {
        Logger(Error) << "error importing pycalimero module";
        //throw PythonException();
        PyErr_Print();
        return;
    }
    cout << "loading module: " << module << endl;
    //FILE *test_py = fopen(module.c_str(), "r");
    boost::format fmt("import %1%");
    fmt % module;
    PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);

    PyObject *pycalimero_dict = PyModule_GetDict(pycalimero_module);
    Py_XDECREF(pycalimero_module);
    PyObject *callback = PyDict_GetItemString(pycalimero_dict, "registerFunctions");

    swig_type_info *reg_type_info = SWIG_TypeQuery("IRegistry *");

    PyObject *py_this = SWIG_NewPointerObj(registry, reg_type_info, 0);

    PyObject *args = Py_BuildValue("(O)", py_this);

    PyObject *res = PyObject_Call(callback, args, Py_None);
    Py_XDECREF(res);
    Py_XDECREF(py_this);
    Py_XDECREF(args);
    if (PyErr_Occurred()) {
        Logger(Error) << "error in python register function";
        //throw PythonException();
        PyErr_Print();
    }
    Logger(Debug) << "successfully loaded python script" << module;


    //ScopedGILRelease scoped;
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
