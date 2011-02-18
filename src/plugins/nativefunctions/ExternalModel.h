#ifndef EXTERNALMODEL_H
#define EXTERNALMODEL_H

#include <vector>
#include <IModelSimulator.h>

class QStringList;
class QString;


CALIMERO_DECLARE_MODELSIMULATOR(ExternalModel)
private:
    QStringList convertArgumentList(QString string);
public:
    ExternalModel();
    bool exec(Domain *dom);
};

#endif // EXTERNALMODEL_H
