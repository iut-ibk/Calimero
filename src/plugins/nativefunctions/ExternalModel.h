#ifndef EXTERNALMODEL_H
#define EXTERNALMODEL_H

#include <vector>
#include <IModelSimulator.h>



CALIMERO_DECLARE_MODELSIMULATOR(ExternalModel)
public:
    ExternalModel();
    bool exec(Domain *dom);
};

#endif // EXTERNALMODEL_H
