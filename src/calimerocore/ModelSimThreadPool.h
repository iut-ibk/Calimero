#ifndef MODELSIMTHREADPOOL_H
#define MODELSIMTHREADPOOL_H

#include <vector>
#include <QThreadPool>
#include <CalimeroGlob.h>
#include <string>
#include <set>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IModelSimulator;
class IterationResult;
class Domain;
class ExternalParameterRegistry;
class Calibration;

using namespace std;

class CALIMERO_PUBLIC ModelSimThreadPool : private QThreadPool
{
public:
    ModelSimThreadPool(int threadnum);
    ~ModelSimThreadPool();
    bool pushIteration( vector<CalibrationVariable*> vars, Calibration *calibration);
};

#endif // MODELSIMTHREADPOOL_H
