#ifndef PYENV_H_INCLUDED
#define PYENV_H_INCLUDED

#include <string>
#include <Registry.h>
#include <IFunction.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>
#include <CalimeroGlob.h>
#include <boost/python.hpp>

using namespace std;
using namespace boost::python;

struct PyEnvPriv;

class CALIMERO_PUBLIC PyEnv {
public:
        virtual ~PyEnv();
        static PyEnv *getInstance();
        static void destroy();
        void addPythonPath(std::string path);
        void registerFunctions(Registry<IObjectiveFunction> *registry, const string &module);
        void registerFunctions(Registry<ICalibrationAlg> *registry, const string &module);
        void registerFunctions(Registry<IModelSimulator> *registry, const string &module);

private:
        PyEnv();
        PyEnvPriv *priv;
        static PyEnv *instance;
};

void wrapPyEnv()
{
    void    (PyEnv::*fx1)(Registry<IObjectiveFunction> *registry, const string &module) = &PyEnv::registerFunctions;
    void    (PyEnv::*fx2)(Registry<ICalibrationAlg> *registry, const string &module) = &PyEnv::registerFunctions;
    void    (PyEnv::*fx3)(Registry<IModelSimulator> *registry, const string &module) = &PyEnv::registerFunctions;

    class_<PyEnv>("PyEnv",no_init)
            .def("addPythonPath", &PyEnv::addPythonPath)
            .def("getInstance", &PyEnv::getInstance, return_value_policy<reference_existing_object>())
            .def("destroy", &PyEnv::destroy)
            .staticmethod("getInstance")
            .staticmethod("destroy")
            .def("registerFunctions", fx1)
            .def("registerFunctions", fx2)
            .def("registerFunctions", fx3)
            ;
}
#endif // PYENV_H_INCLUDED
