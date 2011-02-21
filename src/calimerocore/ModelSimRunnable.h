#ifndef MODELSIMRUNNABLE_H
#define MODELSIMRUNNABLE_H

#include <QRunnable>
#include <vector>
#include <CalimeroGlob.h>
#include <string>
#include <boost/shared_ptr.hpp>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IterationResult;
class IModelSimulator;
class Domain;
class ExternalParameterRegistry;
class IterationResult;
class Calibration;

using namespace std;
using namespace boost;

class CALIMERO_PUBLIC ModelSimRunnable : public QRunnable
{
private:
    vector<string> calibrationparameters;
    vector<string> objectivefunctionparameters;
    vector<string>  iterationparameters;
    vector<string> observedparameters;
    Domain *dom;
    ExternalParameterRegistry *externalfilehandler;
    IterationResult *  result;
    Calibration *calibration;
    IModelSimulator *sim;
    bool error;

public:
    ModelSimRunnable(vector<CalibrationVariable*> newcalpars,Calibration *calibration);
    virtual ~ModelSimRunnable();
    void run();
};

#endif // MODELSIMRUNNABLE_H
