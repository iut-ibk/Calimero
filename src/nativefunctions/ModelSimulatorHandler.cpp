#include <ModelSimulatorHandler.h>
#include <Logger.h>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(ModelSimulatorHandler)

ModelSimulatorHandler::ModelSimulatorHandler()
{
}

void ModelSimulatorHandler::setThreadsNumber(int threads)
{
}

int ModelSimulatorHandler::getThreadsNumber()
{
    return 1;
}

bool ModelSimulatorHandler::exec(Domain *dom)
{
    double index = 0;
    while(index < 500000)
        index++;
    return true;
}
