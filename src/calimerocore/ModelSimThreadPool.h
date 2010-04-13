#ifndef MODELSIMTHREADPOOL_H
#define MODELSIMTHREADPOOL_H

#include <vector>
#include <QThreadPool>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IModelSimulator;
class IterationResult;

using namespace std;

class ModelSimThreadPool : private QThreadPool
{
public:
    ModelSimThreadPool(int threadnum);
    ~ModelSimThreadPool();
    bool pushIteration( vector<CalibrationVariable*> calibrationparameters,
                        vector<Variable*> observedparameters,
                        vector<Variable*> iterationparameters,
                        vector<ObjectiveFunctionVariable*> objectivefunctionparameters,
                        IterationResult *result,
                        IModelSimulator *sim);
};

#endif // MODELSIMTHREADPOOL_H
