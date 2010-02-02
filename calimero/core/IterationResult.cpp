#include "IterationResult.h"
#include "CalibrationVariable.h"
#include "CalculationVariable.h"
#include "Model.h"
#include <iostream>

using namespace std;

/** @brief getIterationIndex
  *
  * @todo: document this function
  */
int IterationResult::getIterationIndex()
{
    return iterationnr;
}

/** @brief ~IterationResult
  *
  * @todo: document this function
  */
IterationResult::~IterationResult()
{
    delete mutex;
}

IterationResult::IterationResult(Model *m, int index, bool saveall)
{
    qDebug("IterationResult::IterationResult()");
    mutex = new QMutex(QMutex::Recursive);
    iterationnr=index;
     complete= false;
    //extract inputparameters
     QVector<CalibrationVariable*> inputvars = m->getAllCalibrationParameters();

    for(int counter=0; counter<inputvars.size(); counter++)
    {
        CalibrationVariable *tmpvar = inputvars.at(counter);
        input[tmpvar->getName()]=QVector<double>(tmpvar->getValues());
    }

    if(saveall)
        updateIterationParameters(m);
}

/** @brief getValueOfIterationParameter
  *
  * @todo: document this function
  */
QVector<double> IterationResult::getValueOfIterationParameter(QString name)
{
    QMutexLocker locker(mutex);
    if(!complete)
        qFatal("IterationResult is not complete");

    return iteration.value(name);
}

/** @brief getValueOfInputParameter
  *
  * @todo: document this function
  */
QVector<double> IterationResult::getValueOfInputParameter(QString name)
{
    QMutexLocker locker(mutex);
    return input.value(name);
}

/** @brief getValueOfCompareParameter
  *
  * @todo: document this function
  */
QVector<double> IterationResult::getValueOfCompareParameter(QString name)
{
    QMutexLocker locker(mutex);
    if(!complete)
        qFatal("IterationResult is not complete");

    return compare.value(name);
}

void IterationResult::updateIterationParameters(Model *m)
{
    QMutexLocker locker(mutex);
    if(complete)
        qFatal("IterationResult is already complete");

    //extract iterationparameters
    QVector<Variable*> iterationvars = m->getAllIterationParameters();

    for(int counter=0; counter<iterationvars.size(); counter++)
    {
        Variable *tmpvar = iterationvars.at(counter);
        iteration[tmpvar->getName()]=QVector<double>(tmpvar->getValues());
    }

    //extract compareparameters
    QVector<CalculationVariable*> comparevars = m->getAllCompareParameters();

    for(int counter=0; counter<comparevars.size(); counter++)
    {
        Variable *tmpvar = comparevars.at(counter);
        compare[tmpvar->getName()]=QVector<double>(tmpvar->getValues());
    }

    complete=true;
}

bool IterationResult::isComplete()
{
    QMutexLocker locker(mutex);
    return complete;
}

QVector<double> IterationResult::getValueOfParameter(QString name)
{
    QMutexLocker locker(mutex);
    if(compare.contains(name))
        return compare.value(name);

    if(input.contains(name))
        return input.value(name);

    if(iteration.contains(name))
        return iteration.value(name);

    return QVector<double>();
}



