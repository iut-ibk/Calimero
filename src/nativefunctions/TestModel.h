#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <vector>
#include <IModelSimulator.h>

CALIMERO_DECLARE_MODELSIMULATOR(TestModel)
public:
    TestModel();
    bool exec(Domain *dom);
};

#endif // TESTMODEL_H
