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

ModelSimThreadPool::ModelSimThreadPool(int threadnum,bool disableautothreads)
{
    this->disableautothreads=disableautothreads;

    if(disableautothreads)
        this->setMaxThreadCount(0);
    else
        this->setMaxThreadCount(threadnum);
}

ModelSimThreadPool::~ModelSimThreadPool()
{
}

bool ModelSimThreadPool::pushIteration( vector<CalibrationVariable*> vars, Calibration *calibration)
{
    QMutexLocker *locker = new QMutexLocker(&mutex);
    ModelSimRunnable *simulation = new ModelSimRunnable(vars,calibration);
    delete locker;
    simulation->setAutoDelete(true);

    if(disableautothreads)
    {
        if(!CalibrationEnv::getInstance()->isCalibrationRunning())
        {
            delete simulation;
            return false;
        }

        simulation->run();
        return true;
    }
    else
    {
        start(simulation);
    }
    return true;
}
