#include <ModelSimThreadPool.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IModelSimulator.h>
#include <IterationResult.h>
#include <Logger.h>


ModelSimThreadPool::ModelSimThreadPool(int threadnum)
{
    this->setMaxThreadCount(threadnum);
}

ModelSimThreadPool::~ModelSimThreadPool()
{
}

bool ModelSimThreadPool::pushIteration( vector<CalibrationVariable*> calibrationparameters,
                    vector<Variable*> observedparameters,
                    vector<Variable*> iterationparameters,
                    vector<ObjectiveFunctionVariable*> objectivefunctionparameters,
                    IterationResult *result,
                    IModelSimulator *sim)
{
    Logger(Error) << "TODO ==================== pushIteration not implemented in ModelSimThreadPool";
    return false;
}

