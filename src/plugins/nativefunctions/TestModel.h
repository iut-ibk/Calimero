#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <vector>
#include <IModelSimulator.h>

CALIMERO_DECLARE_MODELSIMULATOR(TestModel)
public:
    TestModel();
    bool exec(Domain *dom);
};

CALIMERO_DECLARE_MODELSIMULATOR(VectorModel)
public:
    VectorModel();
    bool exec(Domain *dom);
};

double fac(int value);

CALIMERO_DECLARE_MODELSIMULATOR(Schmutzstoffmodell)
public:
    Schmutzstoffmodell();
    bool exec(Domain *dom);
};


#endif // TESTMODEL_H
