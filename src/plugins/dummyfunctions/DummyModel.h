#ifndef DummyModel_H
#define DummyModel_H

#include <vector>
#include <IModelSimulator.h>

CALIMERO_DECLARE_MODELSIMULATOR(DummyModel)
public:
    DummyModel();
    bool exec(Domain *dom);
    int fac(int x);
};

#endif // DummyModel_H
