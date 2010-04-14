#ifndef MODELSIMTHREADPOOL_H
#define MODELSIMTHREADPOOL_H

#include <vector>
#include <QThreadPool>
#include <CalimeroGlob.h>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IModelSimulator;
class IterationResult;

using namespace std;

class CALIMERO_PUBLIC ModelSimThreadPool : private QThreadPool
{
public:
    ModelSimThreadPool(int threadnum);
    ~ModelSimThreadPool();
    bool pushIteration( vector<CalibrationVariable*> calibrationparameters);
};

#endif // MODELSIMTHREADPOOL_H
