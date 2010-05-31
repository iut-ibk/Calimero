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
#include <BruteForceSearch.h>

//Model simulator includes
#include <ExternalModel.h>
#include <TestModel.h>

extern "C"
{
    void CALIMERO_PUBLIC registerObjectiveFunctions(Registry<IObjectiveFunction> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<TestOFunction>());
    }

    void CALIMERO_PUBLIC registerCalibrationAlgs(Registry<ICalibrationAlg> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<TestCalibrationAlg>());
        registry->registerFunction(new NativeFunctionFactory<BruteForceSearch>());
    }

    void CALIMERO_PUBLIC registerModelSimulations(Registry<IModelSimulator> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<ExternalModel>());
        registry->registerFunction(new NativeFunctionFactory<TestModel>());
    }
}
