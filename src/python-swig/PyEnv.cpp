#define SWIG_PYTHON_THREADS

#include <boost/format.hpp>
#include <string>
#include <Registry.h>
#include <Calibration.h>
#include <CalibrationEnv.h>
#include <iostream>
#include <boost/foreach.hpp>
#include <list>
#include <PyEnv.h>
#include <iostream>
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
    result = CalibrationEnv::getInstance()->execIteration(calibrationparameters);
    return result;
}

void barrier()
{
    CalibrationEnv::getInstance()->barrier();
};

struct PyEnvPriv {
        PyObject *main_namespace;
};

PyEnv *PyEnv::instance = 0;


PyEnv::PyEnv() {
        pythonmainthread=false;
        priv = new PyEnvPriv();

        if(!Py_IsInitialized()) {
            Py_Initialize();
            SWIG_PYTHON_INITIALIZE_THREADS;
            PyThreadState *pts = PyGILState_GetThisThreadState();
            PyEval_ReleaseThread(pts);
        }

        SWIG_PYTHON_THREAD_BEGIN_BLOCK;
        PyObject *main = PyImport_ImportModule("__main__");
        priv->main_namespace = PyModule_GetDict(main);
        Py_DECREF(main);

        //redirect stdout and stderr
        boost::format fmt( "import sys\n"
                                   "class Logger:\n"
                                   "    def __init__(self, stdout,error):\n"
                                   "        self.stdout = stdout\n"
                                   "        self.error=error\n"
                                   "        self.currentstring=\"\"\n"
                                   "\n"
                                   "    def write(self, text):\n"
                                   "        self.stdout.write(text)\n"
                                   "        self.currentstring = self.currentstring + \" \" + text\n"
                                   "\n"
                                   "        if text.rfind(\"\\n\") == -1:\n"
                                   "                return\n"
                                   "\n"
                                   "        self.currentstring=self.currentstring.replace(\"\\n\",\"\")\n"
                                   "        self.currentstring=self.currentstring.replace(\"\\r\",\"\")\n"
                                   "        if self.error:\n"
                                   "                pycalimero.log(self.currentstring,pycalimero.Error)\n"
                                   "        else:\n"
                                   "                pycalimero.log(self.currentstring,pycalimero.Standard)\n"
                                   "        self.currentstring=\"\"\n"
                                   "\n"
                                   "    def close(self):\n"
                                   "        self.stdout.close()\n"
                                   "\n"
                                   "if not isinstance(sys.stdout,Logger):\n"
                                   "        sys.stdout=Logger(sys.stdout,False)\n"
                                   "        sys.stderr=Logger(sys.stderr,True)\n");


        PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
        if (PyErr_Occurred()) {
            PyErr_Print();
            return;
        }

        #if defined(SWIG_PYTHON_THREADS)
            Logger(Standard) << "Pythonthreads enabled";
        #else
            Logger(Standard) << "Pythonthreads disabled";
        #endif

        #if defined(OPENMP_ENABLED)
            Logger(Standard) << "OMP enabled";
        #else
            Logger(Standard) << "OMP disabled";
        #endif
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
        SWIG_PYTHON_THREAD_BEGIN_BLOCK;
        PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
}

void PyEnv::registerFunctions(IRegistry *registry, const string &module,bool import)
{
    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyObject *pycalimero_module = PyImport_ImportModule("pycalimero");
    if (PyErr_Occurred()) {
        Logger(Error) << "Could not import pycalimero module";
        PyErr_Print();
        return;
    }

    
    boost::format fmt;
    if(import)
    {
        if(loadedmodules.find(module)==loadedmodules.end())
        {
            Logger(Standard) << "Importing new module: " << module;
            fmt = boost::format("import %1%");
        }
        else
        {
            Logger(Standard) << "Reloading existing module: " << module;
            fmt = boost::format("reload(%1%)");
        }

        fmt % module;
        PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
        if (PyErr_Occurred())
        {
            PyErr_Print();
            throw CalimeroException("Error in python module: " + module);
            return;
        }
    }

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
    if (PyErr_Occurred())
    {
        PyErr_Print();
        return;
    }

    loadedmodules.insert(module);
}