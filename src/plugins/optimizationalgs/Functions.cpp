//general includes
#include <CalimeroGlob.h>
#include <Registry.h>
#include <IFunctionFactory.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>

//Calibration algorithm includes
#include <GeneticAlgorithm.h>
#include <ParticleSwarmAlgorithm.h>

extern "C"
{
    void CALIMERO_PUBLIC registerCalibrationAlgs(Registry<ICalibrationAlg> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<GeneticAlgorithm>());
        registry->registerFunction(new NativeFunctionFactory<ParticleSwarmAlgorithm>());
    }
}
