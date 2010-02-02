#include "Variable.h"
#include "CalculationVariable.h"

Variable::Variable(QString Name, QVector<double> value, int TYPE)
{
    name=Name;
    values+=value;
    type = TYPE;
    filename="";
    mutex = new QMutex(QMutex::Recursive);
}

Variable::~Variable()
{
    for(int index=0; index<monitor.size(); index++)
        if(monitor[index]!=NULL)
        {
            monitor[index]->removeIterationParameter(this);
            monitor[index]->removeResultParameter(this);
        }
}

QString Variable::getName()
{
    QMutexLocker locker(mutex);
    return name;
}

double Variable::getCurrentValue()
{
    QMutexLocker locker(mutex);
    return getValues()[0];
}

void Variable::setName(QString name)
{
    QMutexLocker locker(mutex);
    this->name=name;
    emit variablechanged();
}

void Variable::setCurrentValue(double value)
{
    QMutexLocker locker(mutex);
    values[0]=value;
    fireUpdate();
    emit variablechanged();
}

QVector<double> Variable::getValues()
{
    QMutexLocker locker(mutex);
    return QVector<double>(values);
}

void Variable::setValues(QVector<double> value)
{
    QMutexLocker locker(mutex);
    values.clear();
    values+=value;
    fireUpdate();
    emit variablechanged();
}

int Variable::getType()
{
    return type;
}

QString Variable::getFileName()
{
    QMutexLocker locker(mutex);
    return filename;
}

void Variable::setFileName(QString name)
{
    QMutexLocker locker(mutex);
    filename=name;
}

void Variable::addMonitor(CalculationVariable *var)
{
    QMutexLocker locker(mutex);
    monitor.append(var);
}

void Variable::removeMonitor(CalculationVariable *var)
{
    QMutexLocker locker(mutex);
    if(var==NULL)
        return;

    if(monitor.indexOf(var)>=0)
        monitor.remove(monitor.indexOf(var));
}

void Variable::fireUpdate()
{
    QMutexLocker locker(mutex);
    for(int index=0; index<monitor.size(); index++)
        monitor[index]->calc(this);
}
