#include <ModelSimThreadPool.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IModelSimulator.h>
#include <IterationResult.h>
#include <Logger.h>
#include <ExternalParameterRegistry.h>
#include <Domain.h>
#include <ModelSimRunnable.h>
#include <Calibration.h>
#include <CalibrationEnv.h>


ModelSimThreadPool::ModelSimThreadPool(int threadnum)
{
    this->setMaxThreadCount(threadnum);
}

ModelSimThreadPool::~ModelSimThreadPool()
{
}

bool ModelSimThreadPool::pushIteration( vector<CalibrationVariable*> vars, Calibration *calibration)
{
    ModelSimRunnable *simulation = new ModelSimRunnable(vars,calibration);
    simulation->setAutoDelete(true);

    while(!tryStart(simulation));
    {
        if(!CalibrationEnv::getInstance()->isCalibrationRunning())
        {
            delete simulation;
            return false;
        }
    }

    return true;
}
