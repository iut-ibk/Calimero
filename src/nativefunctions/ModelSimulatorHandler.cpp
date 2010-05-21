#include <ModelSimulatorHandler.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>

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
    CalibrationVariable* pi = static_cast<CalibrationVariable*>(dom->getAllPars(CALIBRATIONVARIABLE).at(0));
    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);
    vector<double> result = iterationresult->getValues();
    result[0]=pi->getValues()[0]*5;
    iterationresult->setValues(result);
    return true;
}
