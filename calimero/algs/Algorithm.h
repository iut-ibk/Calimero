#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "../core/CoreEngine.h"
#include <QVector>

class Algorithm
{
protected:
        CoreEngine *engine;
public:
        Algorithm(CoreEngine *e){if(e!=NULL) engine=e;}
        virtual int runIteration(QVector<CalibrationVariable*> calibrationparameters, bool *ok=NULL) = 0;
        virtual bool setIterationResult(QVector<double> objectivefunctionvalues, int iterationindex) = 0;
        virtual bool init(QVector<CalibrationVariable*> calibrationparameters, QStringList *list=NULL) = 0;
};

#endif // ALGORITHM_H
