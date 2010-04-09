//general includes
#include <CalimeroGlob.h>
#include <Registry.h>
#include <IFunctionFactory.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>

//Objective function includes
#include <TestFunction.h>

//Calibration algorithm includes
#include <TestCalibrationAlg.h>

//Model simulator includes
#include <ModelSimulatorHandler.h>

extern "C"
{
    void CALIMERO_PUBLIC registerObjectiveFunctions(Registry<IObjectiveFunction> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<TestOFunction>());
    }

    void CALIMERO_PUBLIC registerCalibrationAlgs(Registry<ICalibrationAlg> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<TestCalibrationAlg>());
    }

    void CALIMERO_PUBLIC registerModelSimulations(Registry<IModelSimulator> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<ModelSimulatorHandler>());
    }
}
