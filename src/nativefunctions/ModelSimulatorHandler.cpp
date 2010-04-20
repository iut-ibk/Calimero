#include <ModelSimulatorHandler.h>
#include <Logger.h>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(ModelSimulatorHandler)

ModelSimulatorHandler::ModelSimulatorHandler()
{
    Logger(Error) << "ModelSimulatorHandler not implemented";
}

void ModelSimulatorHandler::setThreadsNumber(int threads)
{
    Logger(Error) << "ModelSimulatorHandler not implemented";
}

bool ModelSimulatorHandler::exec(Domain *dom)
{
    Logger(Error) << "ModelSimulatorHandler not implemented";
    return false;
}
