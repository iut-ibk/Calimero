#ifndef MODELSIMRUNNABLE_H
#define MODELSIMRUNNABLE_H

#include <QRunnable>
#include <vector>
#include <CalimeroGlob.h>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IterationResult;
class IModelSimulator;

using namespace std;

class CALIMERO_PUBLIC ModelSimRunnable : public QRunnable
{
private:
    vector<CalibrationVariable*> calibrationparameters;
    vector<Variable*> observedparameters;
    vector<Variable*> iterationparameters;
    vector<ObjectiveFunctionVariable*> objectivefunctionparameters;
    IterationResult *result;
    IModelSimulator *sim;
public:
    ModelSimRunnable(const vector<CalibrationVariable*> &calibrationparameters);
    void run();

private:
    Variable* cloneParameter(Variable* old);
};

#endif // MODELSIMRUNNABLE_H
