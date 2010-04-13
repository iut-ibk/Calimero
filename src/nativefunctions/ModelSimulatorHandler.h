#ifndef MODELSIMULATORHANDLER_H
#define MODELSIMULATORHANDLER_H

#include <vector>
#include <IModelSimulator.h>



CALIMERO_DECLARE_MODELSIMULATOR(ModelSimulatorHandler)
private:
    int threats;
public:
    ModelSimulatorHandler();
    void setThreadsNumber(int threads);
    bool exec(vector<CalibrationVariable*> calibrationparameters,
              vector<Variable*> observedparameters,
              vector<Variable*> iterationparameters,
              vector<ObjectiveFunctionVariable*> objectivefunctionparameters, IterationResult *result);
};

#endif // MODELSIMULATORHANDLER_H
