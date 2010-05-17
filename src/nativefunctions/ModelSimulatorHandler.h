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
    int getThreadsNumber();
    bool exec(Domain *dom);
};

#endif // MODELSIMULATORHANDLER_H
