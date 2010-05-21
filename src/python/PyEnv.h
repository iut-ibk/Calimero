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

void handle_python_exception(const std::string &msg);

struct PyEnvPriv;

class CALIMERO_PUBLIC ScopedGILRelease
{
public:
    ScopedGILRelease()
    {
             m_gstate = PyGILState_Ensure();
    }

    ~ScopedGILRelease()
    {
            PyGILState_Release(m_gstate);
    }

private:
    PyGILState_STATE m_gstate;
};

class CALIMERO_PUBLIC PyEnv {
public:
        virtual ~PyEnv();
        static PyEnv *getInstance();
        static void destroy();
        void addPythonPath(std::string path);
        void registerFunctions(IRegistry *registry, const string &module);

private:
        PyEnv();
        PyEnvPriv *priv;
        static PyEnv *instance;
};

void wrapPyEnv();
#endif // PYENV_H_INCLUDED
