#ifndef PYENV_H_INCLUDED
#define PYENV_H_INCLUDED

#include <string>
#include <Registry.h>
#include <IFunction.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>
#include <CalimeroGlob.h>
#include <set>
#include <QThread>
#include <vector>

using namespace std;

struct PyEnvPriv;

/*
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
*/

class CALIMERO_PUBLIC PyEnv {
public:
        bool pythonmainthread;
        //PyGILState_STATE m_gstate;

        virtual ~PyEnv();
        static PyEnv *getInstance();
        static void destroy();
        void addPythonPath(std::string path);
        void registerFunctions(IRegistry *registry, const string &module);

private:
        PyEnv();
        PyEnvPriv *priv;
        static PyEnv *instance;
        set<string> loadedmodules;
};
bool execIteration(vector<CalibrationVariable*> calibrationparameters);
void barrier();
#endif // PYENV_H_INCLUDED
