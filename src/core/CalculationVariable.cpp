#include "CalculationVariable.h"
#include "../qtscript/ScriptLibary.h"
#include <QMutex>
#include <QMutexLocker>
#include <boost/python.hpp>


CalculationVariable::CalculationVariable(QString Name) : Variable(Name, QVector<double>(),Variable::CALCULATIONVARIABLE)
{
    qDebug("CalculationVariable::CalculationVariable()");
    algname="";
    this->setObjectName(Name);
    iterationparameter = new QVector<Variable*>();
    resultparameter = new QVector<Variable*>;
    compareparameter = new QVector<CalculationVariable*>;
    needupdate = false;
    enableupdate=true;
}

CalculationVariable::~CalculationVariable()
{
    qDebug("CalculationVariable::~CalculationVariable()");
    for(int index = 0; index<iterationparameter->size(); index++)
        if(iterationparameter->at(index)!=NULL)
            iterationparameter->at(index)->removeMonitor(this);

    for(int index = 0; index<resultparameter->size(); index++)
        if(resultparameter->at(index)!=NULL)
            resultparameter->at(index)->removeMonitor(this);

    for(int index = 0; index<compareparameter->size(); index++)
        if(compareparameter->at(index)!=NULL)
            compareparameter->at(index)->removeMonitor(this);

    for(int index=0; index<monitor.size(); index++)
        if(monitor[index]!=NULL)
        {
            monitor[index]->removeCompareParameter(this);
        }

    delete iterationparameter;
    delete resultparameter;
    delete compareparameter;
}

bool CalculationVariable::parameterCycleCheck(CalculationVariable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::parameterCycleCheck()");
    if(this==var)
            return false;

    if(compareparameter->contains(var))
    {
        return false;
    }
    else
    {
        for(int counter=0; counter<compareparameter->size(); counter++)
        {
            if(!compareparameter->at(counter)->parameterCycleCheck(var))
            {
                return false;
            }
        }
    }

    return true;
}

bool CalculationVariable::addCompareParameter(CalculationVariable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::addCompareParameter()");
    if(var->parameterCycleCheck(this))
    {
        compareparameter->append(var);
        var->addMonitor(this);
        needupdate=true;
        emit variablechanged();
        return true;
    }
    else
    {
        return false;
    }
}

bool CalculationVariable::addResultParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::addResultParameter()");
    if(var!=NULL)
    {
        resultparameter->append(var);
        var->addMonitor(this);
        needupdate=true;
        emit variablechanged();
        return true;
    }
    else
    {
        return false;
    }
}

bool CalculationVariable::addIterationParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::addIterationParameter()");
    if(var!=NULL)
    {
        iterationparameter->append(var);
        var->addMonitor(this);
        needupdate=true;
        emit variablechanged();
        return true;
    }
    else
    {
        return false;
    }
}

bool CalculationVariable::removeCompareParameter(CalculationVariable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::removeCompareParameter()");
    if(!compareparameter->contains(var))
        return false;

    compareparameter->remove(compareparameter->indexOf(var));
    var->addMonitor(this);
    needupdate=true;
    emit variablechanged();
    return true;
}

bool CalculationVariable::removeIterationParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::removeIterationParameter()");
    if(!iterationparameter->contains(var))
        return false;

    iterationparameter->remove(iterationparameter->indexOf(var));
    var->addMonitor(this);
    needupdate=true;
    emit variablechanged();
    return true;
}

bool CalculationVariable::removeResultParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::removeResultParameter()");
    if(!resultparameter->contains(var))
        return false;

    resultparameter->remove(resultparameter->indexOf(var));
    var->addMonitor(this);
    needupdate=true;
    emit variablechanged();
    return true;
}

int CalculationVariable::sizeOfCompareParameter()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::srizeOfCompareParameter()");
    return compareparameter->size();
}

int CalculationVariable::sizeOfIterationParameter()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::sizeOfIterationParameter()");
    return iterationparameter->size();
}

int CalculationVariable::sizeOfResultParameter()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::sizeOfIterationParameter()");
    return resultparameter->size();
}

CalculationVariable* CalculationVariable::atCompareParameter(int index)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::sizeOfResultParameter()");
    if(index>=compareparameter->size())
        return NULL;

    return compareparameter->at(index);
}

Variable* CalculationVariable::atIterationParameter(int index)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::atIterationParameter()");
    if(index>=iterationparameter->size())
        return NULL;

    return iterationparameter->at(index);
}

Variable* CalculationVariable::atResultParameter(int index)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::atResultParameter()");
    if(index>=resultparameter->size())
        return NULL;

    return resultparameter->at(index);
}

bool CalculationVariable::containsIterationParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::containsIterationParameter()");
    return iterationparameter->contains(var);
}

bool CalculationVariable::containsResultParameter(Variable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::containsResultParameter()");
    return resultparameter->contains(var);
}

bool CalculationVariable::containsCompareParameter(CalculationVariable* var)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::containsCompareParameter()");
    return compareparameter->contains(var);
}

void CalculationVariable::setAlg(QString name)
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::setAlg()");
    if(name!="")
    {
        algname=QString(name);
        needupdate=true;
        emit variablechanged();
    }
}

QString CalculationVariable::getAlg()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::getAlg()");
    return algname;
}

QScriptEngine* CalculationVariable::getScriptEngine()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::getScriptEngine()");
    QScriptEngine *engine = new QScriptEngine();
    qScriptRegisterMetaType(engine, ScriptLibary::variableToScriptValue, ScriptLibary::variableFromScriptValue);
    qScriptRegisterMetaType(engine, ScriptLibary::calculationVariableToScriptValue, ScriptLibary::calculationVariableFromScriptValue);
    engine->setProcessEventsInterval(10);
    engine->globalObject().setProperty("iterationparameter", qScriptValueFromSequence(engine,*iterationparameter));
    engine->globalObject().setProperty("compareparameter", qScriptValueFromSequence(engine,*compareparameter));
    engine->globalObject().setProperty("resultparameter", qScriptValueFromSequence(engine,*resultparameter));
    engine->globalObject().setProperty("parameter", qScriptValueFromSequence(engine,values));

    return engine;
}

void CalculationVariable::calc(Variable *client)
{
    QMutexLocker locker(mutex);

    if(!enableupdate)
        return;

    if(client!=this)
    {
        fireUpdate();
        needupdate=true;
        return;
    }
    else
    {
        if(!needupdate)
        {
            return;
        }
    }

    qDebug() << "CalculationVariable::calc()";
    ScriptLibary *libary = ScriptLibary::getInstance();
    qDebug() << "CalculationVariable::calc() searching for alg:" << algname;

    if(!libary->existsCompareScript(algname))
    {
        qDebug("CalculationVariable::calc() noalg");
        setCurrentValue(-9999);
        return;
    }

    qDebug("CalculationVariable::calc() STAGE 1");
    QString fileName = ScriptLibary::getInstance()->getCompareScriptPath()  + algname + "/main.qs";

    if(!QFile::exists(fileName))
    {
        qWarning("CalculationVariable::calc() ERROR");
        return;
    }
    qDebug("CalculationVariable::calc() STAGE 2");
    QFile scriptFile(fileName);

    if(!scriptFile.open(QIODevice::ReadOnly))
    {
        qWarning("Loading script file faild");
        return;
    }
    qDebug("CalculationVariable::calc() STAGE 3");
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    QScriptEngine *engine = getScriptEngine();
    QScriptValue include = engine->newFunction(ScriptLibary::include);
    include.setProperty("WORKINGDIR", ScriptLibary::getInstance()->getCompareScriptPath()  + algname);
    engine->globalObject().setProperty("include", include);
    QScriptValue print = engine->newFunction(ScriptLibary::printLog);
    engine->globalObject().setProperty("print", print);
    engine->evaluate(contents,algname);

    values.clear();
    qScriptValueToSequence(engine->globalObject().property("parameter"),values);
    if(engine->isEvaluating())
        engine->abortEvaluation();
    delete engine;
    needupdate=false;
    qDebug() << "CalculationVariable::calc() DONE First element of result vector=" << values.at(0);

    using namespace boost::python;

    try {
        Py_Initialize();
        object main_module = import("__main__");
        object main_namespace = main_module.attr("__dict__");
        object ignored = exec("print 'Hallo Welt from python'\n",
                          main_namespace);
    } catch(error_already_set const &) {
        PyErr_Print();
    }
}

double CalculationVariable::getCurrentValue()
{
    QMutexLocker locker(mutex);
    qDebug("CalculationVariable::getCurrentValue()");
    return getValues()[0];
}

void CalculationVariable::setCurrentValue(double value)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(value);
    qFatal("CalculationVariable::setCurrentValue()");
    return;
}

QVector<double>  CalculationVariable::getValues()
{
    QMutexLocker locker(mutex);
    calc(this);
    return QVector<double>(values);
}

void CalculationVariable::setValues(QVector<double> value)
{
    QMutexLocker locker(mutex);
    enableupdate=false;
    values=value;
}

void CalculationVariable::enableUpdate(bool enable)
{
    QMutexLocker locker(mutex);
    enableupdate=enable;
    needupdate=true;
}

