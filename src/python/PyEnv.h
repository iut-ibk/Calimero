#ifndef PYENV_H_INCLUDED
#define PYENV_H_INCLUDED

#include <string>
#include <Registry.h>
#include <IFunction.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>

using namespace std;


struct PyEnvPriv;

class PyEnv {
public:
        virtual ~PyEnv();
        static PyEnv *getInstance();
        void addPythonPath(std::string path);
        void registerFunctions(Registry<IObjectiveFunction> *registry, const string &module);
        void registerFunctions(Registry<ICalibrationAlg> *registry, const string &module);

private:
        PyEnv();
        PyEnvPriv *priv;
        static PyEnv *instance;
};

#endif // PYENV_H_INCLUDED
