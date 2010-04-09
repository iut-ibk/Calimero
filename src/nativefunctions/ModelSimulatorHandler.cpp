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

bool ModelSimulatorHandler::exec(vector<CalibrationVariable*> calibrationparameters,
          vector<Variable*> observedparameters,
          vector<Variable*> iterationparameters,
          vector<ObjectiveFunctionVariable*> objectivefunctionparameters, IterationResult *result)
{
    Logger(Error) << "ModelSimulatorHandler not implemented";
    return false;
}

void ModelSimulatorHandler::stop()
{
    Logger(Error) << "ModelSimulatorHandler not implemented";
}
