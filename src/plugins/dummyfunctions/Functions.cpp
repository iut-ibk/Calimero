//general includes
#include <CalimeroGlob.h>
#include <Registry.h>
#include <IFunctionFactory.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>

//Objective function includes
#include <DummyFunction.h>

//Calibration algorithm includes

//Model simulator includes
#include <DummyModel.h>

extern "C"
{
    void CALIMERO_PUBLIC registerObjectiveFunctions(Registry<IObjectiveFunction> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<DummyOFunction>());
    }

    void CALIMERO_PUBLIC registerCalibrationAlgs(Registry<ICalibrationAlg> *registry)
    {
    }

    void CALIMERO_PUBLIC registerModelSimulations(Registry<IModelSimulator> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<DummyModel>());
    }
}
