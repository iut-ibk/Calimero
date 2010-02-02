#include "Model.h"
#include <QtXml>
#include "../qtscript/ScriptLibary.h"
#include "CalculationVariable.h"
#include <QtCore>
#include "../persistence/Persistence.h"
#include "CalibrationVariable.h"
#include "../persistence/ParameterPersistenceEvaluator.h"
#include "IterationResult.h"
#include "Variable.h"
#include "CoreEngine.h"
#include "VariableContainer.h"
#include <QtDebug>

Model::Model(CoreEngine *core)
{
    qDebug("Model::Model()");
    mutex = new QMutex(QMutex::Recursive);
    this->setObjectName("model");
    coreengine=NULL;
    maxerror=0;
    cleariterations = true;
    cpunum=1;
    preexec="";
    preexecargument="";
    iterationexec="";
    iterationexecargument="";
    postexec="";
    postexecargument="";
    preworkspace="";
    postworkspace="";
    iterationworkspace="";
    groupnames = new QStringList();
    evaluatinggroups = new QStringList();
    notevaluatinggroups = new QStringList();
    groups = new QVector<QVector<CalibrationVariable*>*>();
    iterationresults = new QVector<IterationResult*>();
    parameterevaluator = new ParameterPersistenceEvaluator();
    groupnames->append("default");
    evaluatinggroups->append(groupnames->at(0));
    groups->append(new QVector<CalibrationVariable*>());
    algparameters = new QVector<CalculationVariable*>();
    maxiterations=0;
    algname="";
    inputvarcontainer = new VariableContainer<CalibrationVariable>(parameterevaluator,Variable::CALIBRATIONVARIABLE);
    calccontainer = new VariableContainer<CalculationVariable>(parameterevaluator,Variable::CALCULATIONVARIABLE);
    resultvarcontainer = new VariableContainer<Variable>(parameterevaluator,Variable::RESULTVARIABLE);
    iterationvarcontainer = new VariableContainer<Variable>(parameterevaluator,Variable::ITERATIONVARIABLE);
    calccontainer->newTemplate("compare","compare");
    coreengine=core;
}

Model::~Model()
{
    qDebug("Model::~Model()");
    clear();
    delete algparameters;
    delete evaluatinggroups;
    delete notevaluatinggroups;
    delete groupnames;
    delete groups;
    delete parameterevaluator;
    delete iterationresults;
    delete inputvarcontainer;
    delete calccontainer;
    delete resultvarcontainer;
    delete iterationvarcontainer;
    delete mutex;
}

QString Model::getTemplate(QString templatename)
{
    QMutexLocker locker(mutex);
    qDebug("Model::getTemplate()");

    QString result = "";

    if(!existsFileName(templatename))
        return result;

    if(inputvarcontainer->containsTemplate(templatename))
        result = inputvarcontainer->getTemplate(templatename);

    if(resultvarcontainer->containsTemplate(templatename))
        result = resultvarcontainer->getTemplate(templatename);

    if(iterationvarcontainer->containsTemplate(templatename))
        result = iterationvarcontainer->getTemplate(templatename);

    return result;
}

Variable* Model::getParameter(int index, QString templatename)
{
    QMutexLocker locker(mutex);
    qDebug("Model::getParameter()");

    if(templatename=="")
        qFatal("Model::getParameter: Templatename is empty");

    if(!existsFileName(templatename))
        qFatal("Model::getParameter: Templatename does not exist");
    
    if(index<0)
        qFatal("Model::getParameter: Wrong index");

    int type = getType(templatename);

    if(type==Variable::CALIBRATIONVARIABLE && index<inputvarcontainer->getVars(templatename)->size())
        return inputvarcontainer->getVars(templatename)->at(index);

    if(type==Variable::RESULTVARIABLE && index<resultvarcontainer->getVars(templatename)->size())
        return resultvarcontainer->getVars(templatename)->at(index);

    if(type==Variable::ITERATIONVARIABLE && index<iterationvarcontainer->getVars(templatename)->size())
        return iterationvarcontainer->getVars(templatename)->at(index);

    qFatal("Model::getParameter(): ERROR -> This code section should never be reached");

    return NULL;
}

bool Model::addTemplate(QString templatename, QString templatestring, int type)
{
    QMutexLocker locker(mutex);
    qDebug("Model::addTemplate()");
    
    if(type < 0 || type > 4)
        return false;

    emit modelchanged(this);

    if(existsFileName(templatename))
        return false;

    if(type == Variable::CALIBRATIONVARIABLE)
        return inputvarcontainer->newTemplate(templatename,templatestring);

    if(type == Variable::ITERATIONVARIABLE)
        return iterationvarcontainer->newTemplate(templatename,templatestring);

    if(type == Variable::RESULTVARIABLE)
        return resultvarcontainer->newTemplate(templatename,templatestring);

    if(type == Variable::CALCULATIONVARIABLE)
        return calccontainer->newTemplate(templatename,templatestring);

    return false;
}

bool Model::existsFileName(QString filename, int type)
{
    QMutexLocker locker(mutex);
    qDebug("Model::existsFileName()");
    
    if(type >= 0 && type <= 4 && type != Variable::CALCULATIONVARIABLE)
    {
        if(type == Variable::CALIBRATIONVARIABLE)
            return inputvarcontainer->containsTemplate(filename);

        if(type == Variable::ITERATIONVARIABLE)
            return iterationvarcontainer->containsTemplate(filename);

        if(type == Variable::RESULTVARIABLE)
            return resultvarcontainer->containsTemplate(filename);
    }
    else
    {
        return inputvarcontainer->containsTemplate(filename) || iterationvarcontainer->containsTemplate(filename) || resultvarcontainer->containsTemplate(filename) || calccontainer->containsTemplate(filename);
    }

    return false;
}

int Model::getType(QString templatename)
{
    QMutexLocker locker(mutex);
    qDebug("Model::getType()");
    if(templatename=="")
        qFatal("Model::getType(): Template name is empty");

    if(!existsFileName(templatename))
        qFatal("Model::getType(): Template does not exist");

    if(inputvarcontainer->containsTemplate(templatename))
        return Variable::CALIBRATIONVARIABLE;

    if(iterationvarcontainer->containsTemplate(templatename))
        return Variable::ITERATIONVARIABLE;

    if(resultvarcontainer->containsTemplate(templatename))
        return Variable::RESULTVARIABLE;

    qFatal("Model::getType(): This codesection should never be reached");
    return -1;
}

bool Model::removeFileName(QString filename, int type)
{
    QMutexLocker locker(mutex);
    qDebug("Model::removeFileName()");
    if(type < 0 || type > 4)
        return false;

    if(filename == "")
        return false;

    if(!existsFileName(filename))
        return false;

    if(type!=getType(filename))
        return false;

    QVector<Variable*> *tmpparameters = NULL;

    if(type == Variable::CALIBRATIONVARIABLE)
    {
        QVector<CalibrationVariable*> *vec;
        vec = inputvarcontainer->getVars(filename);

        while(!vec->isEmpty())
        {
            if(!removeParameter(vec->first()))
                qFatal("Model::removeFileName(): ERROR");
        }

        inputvarcontainer->removeVars(filename);
        emit modelchanged(this);
        return true;
    }

    if(type == Variable::ITERATIONVARIABLE && iterationvarcontainer->containsTemplate(filename))
    {
        tmpparameters = iterationvarcontainer->getVars(filename);
    }

    if(type == Variable::RESULTVARIABLE && resultvarcontainer->containsTemplate(filename))
    {
        tmpparameters = resultvarcontainer->getVars(filename);
    }

    while(!tmpparameters->isEmpty())
    {
         if(!removeParameter(tmpparameters->first()))
             qFatal("Model::removeFileName(): ERROR");
    }

    if(type == Variable::ITERATIONVARIABLE)
        iterationvarcontainer->removeVars(filename);

    if(type == Variable::RESULTVARIABLE)
        resultvarcontainer->removeVars(filename);

    emit modelchanged(this);
    return true;
}

bool Model::importParameter(QString templatepath, QString importfilename, int type)
{
    QMutexLocker locker(mutex);
    qDebug("Model::importParameter()");
    if(templatepath == "")
        return false;

    if(importfilename == "")
        return false;

    if(type < 0 || type > 4)
        return false;

    if(existsFileName(templatepath))
    {
        bool dirty = false;

        if(type == Variable::CALIBRATIONVARIABLE && inputvarcontainer->getVars(templatepath)->size() > 0)
            dirty = true;

        if(type == Variable::ITERATIONVARIABLE && inputvarcontainer->getVars(templatepath)->size() > 0)
            dirty = true;

        if(type == Variable::RESULTVARIABLE && inputvarcontainer->getVars(templatepath)->size() > 0)
            dirty = true;

        if(dirty)
        {
             emit erroroccured(QString(tr("Delete all parameters please\n")));
             return false;
        }
    }

    QString valuestring;

    if(!Persistence::loadFileAsString(importfilename,&valuestring))
        return false;


    QStringList *parameternames = new QStringList();
    QVector<QVector<double>* > *values = new QVector<QVector<double>* >(0);

    if(!parameterevaluator->checkCompatibility(valuestring, getTemplate(templatepath) , parameternames, values))
    {
        emit erroroccured(QString(tr("Import file is incompatible\n")) + importfilename);
        delete parameternames;
        delete values;
        return false;
    }

    if(!removeFileName(templatepath,type))
        qFatal("Model::importParameter(): ERROR");

    if(type == Variable::CALIBRATIONVARIABLE)
    {
        QVector<CalibrationVariable* > * newvector = new QVector<CalibrationVariable* >();

        for(int counter=0; counter<parameternames->size(); counter++)
        {
            CalibrationVariable *tmpvar = new CalibrationVariable(parameternames->at(counter), values->at(counter)->mid(0));
            newvector->append(tmpvar);

            QObject::connect(tmpvar, SIGNAL(variablechanged()), this, SLOT(parameterchanged()));
        }

        if(!inputvarcontainer->addVars(newvector,templatepath,valuestring))
            qFatal("Model::importParameter(): ERROR inster new Parameter vector do VarContainer");

        for(int counter=0; counter<inputvarcontainer->getVars(templatepath)->size(); counter++)
        {
            if(!addParameterToGroup(groupnames->at(0),inputvarcontainer->getVars(templatepath)->at(counter)))
                qFatal("Model::importParameter(): ERROR insert Parameter to default group");
        }
    }
    else
    {
        QVector<Variable* > * newvector = new QVector<Variable* >();

        for(int counter=0; counter<parameternames->size(); counter++)
        {
            Variable *tmpvar = new Variable(parameternames->at(counter), values->at(counter)->mid(0),type);
            newvector->append(tmpvar);
            QObject::connect(tmpvar, SIGNAL(variablechanged()), this, SLOT(parameterchanged()));
        }

        if(type==Variable::ITERATIONVARIABLE)
            if(!iterationvarcontainer->addVars(newvector,templatepath,valuestring))
                qFatal("Model::importParameter(): ERROR insert new Parameter vector to VarContainer");

        if(type==Variable::RESULTVARIABLE)
            if(!resultvarcontainer->addVars(newvector,templatepath,valuestring))
                qFatal("Model::importParameter(): ERROR insert new Parameter vector to VarContainer");
    }

    for(int index=0; index<values->size(); index++)
        delete values->at(index);

    delete parameternames;
    delete values;
    emit modelchanged(this);
    return true;
}

bool Model::removeParameter(Variable *var)
{
    qDebug("Model::removeParameter()");
    QMutexLocker locker(mutex);
    if(!existsParameter(var->getName()))
            return false;

    if(var->getType()==Variable::ITERATIONVARIABLE || var->getType()==Variable::RESULTVARIABLE|| var->getType()==Variable::CALCULATIONVARIABLE)
    {
        Variable *parameter = var;

        QVector<CalculationVariable* > compareparameters = calccontainer->getAllVars();

        for(int counter=0; counter<compareparameters.size(); counter++)
        {
            if(var->getType()==Variable::ITERATIONVARIABLE)
            {
                if(compareparameters.at(counter)->containsIterationParameter(parameter))
                {
                    compareparameters.at(counter)->removeIterationParameter(parameter);
                }
            }

            if(var->getType()==Variable::RESULTVARIABLE)
            {
                if(compareparameters.at(counter)->containsResultParameter(parameter))
                {
                    compareparameters.at(counter)->removeResultParameter(parameter);
                }
            }

            if(var->getType()==Variable::CALCULATIONVARIABLE)
            {
                if(compareparameters.at(counter)->containsCompareParameter(static_cast<CalculationVariable*>(parameter)))
                {
                    compareparameters.at(counter)->removeCompareParameter(static_cast<CalculationVariable*>(parameter));
                }
            }
        }

        QObject::disconnect(parameter, SIGNAL(variablechanged()), this, SLOT(parameterchanged()));

        if(var->getType()==Variable::ITERATIONVARIABLE)
        {
            if(!iterationvarcontainer->removeVar(var->getName()))
                qFatal("Model::deleteParameter(): ERROR delete parameter");
        }

        if(var->getType()==Variable::RESULTVARIABLE)
        {
            if(!resultvarcontainer->removeVar(var->getName()))
                qFatal("Model::deleteParameter(): ERROR delete parameter");
        }

        if(var->getType()==Variable::CALCULATIONVARIABLE)
        {
            if(algparameters->contains(dynamic_cast<CalculationVariable*>(var)))
                algparameters->remove(algparameters->indexOf(dynamic_cast<CalculationVariable*>(var)));
            if(!calccontainer->removeVar(var->getName()))
                qFatal("Model::deleteParameter(): ERROR delete parameter");
        }

        this->clearIterationResults();
        emit modelchanged(this);
        return true;
    }

    if(var->getType()==Variable::CALIBRATIONVARIABLE)
    {
        CalibrationVariable *parameter = static_cast<CalibrationVariable*>(var);
        groups->at(0)->remove(groups->at(0)->indexOf(parameter));
        deleteParameterFromAllGroups(parameter->getName());
        QObject::disconnect(parameter, SIGNAL(variablechanged()), this, SLOT(parameterchanged()));
        if(!inputvarcontainer->removeVar(var->getName()))
            qFatal("Model::deleteParameter(): ERROR delete parameter");

        this->clearIterationResults();
        emit modelchanged(this);

        return true;
    }

    return false;
}

bool Model::addParameter(Variable *var, QString templatestring, QString templatename)
{
    QMutexLocker locker(mutex);
    qDebug("Model::addParameter()");
    if(!existsFileName(templatename))
    {
        emit erroroccured(QString(tr("No corresponding template for parameter")));
        return false;
    }
    
    if(existsParameter(var->getName()))
    {
        emit erroroccured(QString(tr("Parametername already exists")));
        return false;
    }

    if(var->getType()==Variable::CALIBRATIONVARIABLE && inputvarcontainer->containsTemplate(templatename))
    {
        if(!inputvarcontainer->addVar(static_cast<CalibrationVariable*>(var),templatename,templatestring))
            return false;
    }

    if(var->getType()==Variable::ITERATIONVARIABLE && iterationvarcontainer->containsTemplate(templatename))
    {
        if(!iterationvarcontainer->addVar(var,templatename,templatestring))
            return false;
    }

    if(var->getType()==Variable::RESULTVARIABLE && resultvarcontainer->containsTemplate(templatename))
    {
        if(!resultvarcontainer->addVar(var,templatename,templatestring))
            return false;
    }

    if(var->getType()==Variable::CALCULATIONVARIABLE && calccontainer->containsTemplate(templatename))
    {
        if(!calccontainer->addVar(static_cast<CalculationVariable*>(var),"compare","compare"))
            return false;
    }

    if(var->getType()==Variable::CALIBRATIONVARIABLE)
        addParameterToGroup(groupnames->at(0),static_cast<CalibrationVariable*>(var));

    QObject::connect(var, SIGNAL(variablechanged()), this, SLOT(parameterchanged()));

    emit modelchanged(this);
    return true;
}

bool Model::importIterationParameters(QString templatepath, QString importfilename)
{
    QMutexLocker locker(mutex);
    return importParameter(templatepath,importfilename,Variable::ITERATIONVARIABLE);
}

bool Model::importResultParameters(QString templatepath, QString importfilename)
{
    QMutexLocker locker(mutex);
    return importParameter(templatepath,importfilename,Variable::RESULTVARIABLE);
}

bool Model::importInputParameters(QString templatepath, QString importfilename)
{
    QMutexLocker locker(mutex);
    return importParameter(templatepath,importfilename,Variable::CALIBRATIONVARIABLE);
}

Variable * Model::getIterationParameter(int index, QString iterationfilename)
{
    QMutexLocker locker(mutex);
    return getParameter(index, iterationfilename);
}

Variable * Model::getResultParameter(int index, QString resultfilename)
{
    QMutexLocker locker(mutex);
    return getParameter(index, resultfilename);
}

CalibrationVariable * Model::getInputParameter(int index, QString inputfilename)
{
    QMutexLocker locker(mutex);
    return dynamic_cast<CalibrationVariable*>(getParameter(index, inputfilename));
}

bool Model::deleteIterationParameter(QString parametername, QString iterationfilename)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(iterationfilename);
    bool result = removeParameter(getParameter(parametername));
    return result;
}

bool Model::deleteResultParameter(QString parametername, QString resultfilename)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(resultfilename);
    bool result = removeParameter(getParameter(parametername));
    return result;
}

bool Model::deleteInputParameter(QString parametername, QString inputfilename)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(inputfilename);
    bool result = removeParameter(getParameter(parametername));
    return result;
}

bool Model::addIterationParameter(Variable* var, QString iterationtemplate, QString iterationfilename, bool force)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(force);
    if(var->getType()!=Variable::ITERATIONVARIABLE)
        return false;

    return addParameter(var,iterationtemplate,iterationfilename);
}

bool Model::addResultParameter(Variable* var, QString resulttemplate, QString resultfilename, bool force)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(force);
    if(var->getType()!=Variable::RESULTVARIABLE)
        return false;

    return addParameter(var,resulttemplate,resultfilename);
}

bool Model::addInputParameter(CalibrationVariable* var, QString inputtemplate, QString inputfilename, bool force)
{
    QMutexLocker locker(mutex);
    Q_UNUSED(force);
    if(var->getType()!=Variable::CALIBRATIONVARIABLE)
        return false;

    return addParameter(var,inputtemplate,inputfilename);
}

int Model::numberOfIterationParameters(QString iterationfilename)
{
    QMutexLocker locker(mutex);
    return iterationvarcontainer->getVars(iterationfilename)->size();
}

int Model::numberOfResultParameters(QString resultfilename)
{
    QMutexLocker locker(mutex);
    return resultvarcontainer->getVars(resultfilename)->size();
}

int Model::numberOfInputParameters(QString inputfilename)
{
    QMutexLocker locker(mutex);
    return inputvarcontainer->getVars(inputfilename)->size();
}

int Model::numberOfIterationFiles()
{
    QMutexLocker locker(mutex);
    return iterationvarcontainer->getAllTemplateNames().size();
}

int Model::numberOfResultFiles()
{
    QMutexLocker locker(mutex);
    return resultvarcontainer->getAllTemplateNames().size();
}

int Model::numberOfInputFiles()
{
    QMutexLocker locker(mutex);
    return inputvarcontainer->getAllTemplateNames().size();
}

QString Model::getIterationFileName(int index)
{
    QMutexLocker locker(mutex);
    return iterationvarcontainer->getAllTemplateNames().at(index);
}

QString Model::getResultFileName(int index)
{
    QMutexLocker locker(mutex);
    return resultvarcontainer->getAllTemplateNames().at(index);
}

QString Model::getInputFileName(int index)
{
    QMutexLocker locker(mutex);
    return inputvarcontainer->getAllTemplateNames().at(index);
}

bool Model::removeIterationFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return removeFileName(filename,getType(filename));
}

bool Model::removeResultFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return removeFileName(filename,getType(filename));
}

bool Model::removeInputFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return removeFileName(filename,getType(filename));
}

bool Model::addIterationFileName(QString filename, QString text)
{
    QMutexLocker locker(mutex);
    if(!addTemplate(filename,text,Variable::ITERATIONVARIABLE))
    {
        emit erroroccured(QString(tr("File already included [")) + filename + QString("]\n"));
        return false;
    }

    return true;
}

bool Model::addResultFileName(QString filename, QString text)
{
    QMutexLocker locker(mutex);
    if(!addTemplate(filename,text,Variable::RESULTVARIABLE))
    {
        emit erroroccured(QString(tr("File already included [")) + filename + QString("]\n"));
        return false;
    }

    return true;
}

bool Model::addInputFileName(QString filename, QString text)
{
    QMutexLocker locker(mutex);
    if(!addTemplate(filename,text,Variable::CALIBRATIONVARIABLE))
    {
        emit erroroccured(QString(tr("File already included [")) + filename + QString("]\n"));
        return false;
    }

    return true;
}

bool Model::isPersistent()
{
    QMutexLocker locker(mutex);
    qDebug("Model::isPersistent()");
    return persistent;
}

bool Model::modelSave(QString filename)
{
    QMutexLocker locker(mutex);
    qDebug("Model::modelSave()");
    return Persistence::saveModel(filename, this);
}

int Model::modelLoad(QString filename, bool forcescriptupdate)
{
    QMutexLocker locker(mutex);
    qDebug("Model::modelLoad()");
    clear();
    return Persistence::loadModel(filename, this,forcescriptupdate);
}

void Model::clear()
{
    qDebug("Model::clear()");
    QMutexLocker locker(mutex);

    if(algparameters!=NULL)
        delete algparameters;

    if(evaluatinggroups!=NULL)
        delete evaluatinggroups;

    if(notevaluatinggroups!=NULL)
        delete notevaluatinggroups;

    if(groupnames!=NULL)
        delete groupnames;

    if(groups!=NULL)
        delete groups;

    if(parameterevaluator!=NULL)
        delete parameterevaluator;

    if(iterationresults!=NULL)
        delete iterationresults;

    if(calccontainer!=NULL)
        delete calccontainer;

    if(inputvarcontainer!=NULL)
        delete inputvarcontainer;

    if(resultvarcontainer!=NULL)
        delete resultvarcontainer;

    if(iterationvarcontainer!=NULL)
        delete iterationvarcontainer;

    maxerror=0;
    cleariterations = true;
    cpunum=1;
    preexec="";
    preexecargument="";
    iterationexec="";
    iterationexecargument="";
    postexec="";
    postexecargument="";
    preworkspace="";
    iterationworkspace="";
    postworkspace="";
    groupnames = new QStringList();
    evaluatinggroups = new QStringList();
    notevaluatinggroups = new QStringList();
    groups = new QVector<QVector<CalibrationVariable*>*>();
    iterationresults = new QVector<IterationResult*>();
    parameterevaluator = new ParameterPersistenceEvaluator();
    groupnames->append("default");
    evaluatinggroups->append(groupnames->at(0));
    groups->append(new QVector<CalibrationVariable*>());
    algparameters = new QVector<CalculationVariable*>();
    maxiterations=0;
    algname="";
    inputvarcontainer = new VariableContainer<CalibrationVariable>(parameterevaluator,Variable::CALIBRATIONVARIABLE);
    calccontainer = new VariableContainer<CalculationVariable>(parameterevaluator,Variable::CALCULATIONVARIABLE);
    resultvarcontainer = new VariableContainer<Variable>(parameterevaluator,Variable::RESULTVARIABLE);
    iterationvarcontainer = new VariableContainer<Variable>(parameterevaluator,Variable::ITERATIONVARIABLE);
    calccontainer->newTemplate("compare","compare");

    setMaxIterations(100);
    setCpus(1);
    setAlg("default");
    setPreWorkspace("");
    setPostWorkspace("");
    setIterationWorkspace("");
    setPreExec("");
    setPreExecArgument("");
    setIterationExec("");
    setIterationExecArgument("");
    setPostExec("");
    setPostExecArgument("");
    setClearIterations(true);
}

void Model::parameterchanged()
{
    QMutexLocker locker(mutex);
    emit modelchanged(this);
}

QString Model::getResultTemplate(QString resultFileName)
{
    QMutexLocker locker(mutex);
    return getTemplate(resultFileName);
}

QString Model::getInputTemplate(QString inputfilename)
{
    QMutexLocker locker(mutex);
    return getTemplate(inputfilename);
}

QString Model::getIterationTemplate(QString iterationfilename)
{
    QMutexLocker locker(mutex);
    return getTemplate(iterationfilename);
}

bool Model::existsIterationFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return existsFileName(filename,Variable::ITERATIONVARIABLE);
}

bool Model::existsResultFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return existsFileName(filename,Variable::RESULTVARIABLE);
}

bool Model::existsInputFileName(QString filename)
{
    QMutexLocker locker(mutex);
    return existsFileName(filename,Variable::CALIBRATIONVARIABLE);
}

bool Model::isFreeParameterName(QString name)
{
    QMutexLocker locker(mutex);
    return parameterevaluator->isFreeParameterName(name);
}

QVector<CalibrationVariable*> Model::getAllCalibrationParameters()
{
    QMutexLocker locker(mutex);
    return inputvarcontainer->getAllVars();
}

bool Model::deleteCompareParameter(QString parametername)
{
    QMutexLocker locker(mutex);
    return removeParameter(getParameter(parametername));
}

bool Model::addCompareParameter(CalculationVariable* var)
{
    QMutexLocker locker(mutex);
    if(var->getType()!=Variable::CALCULATIONVARIABLE)
        return false;

    return addParameter(var,"compare","compare");
}

QVector<Variable*> Model::getAllIterationParameters()
{
    QMutexLocker locker(mutex);
    return iterationvarcontainer->getAllVars();
}

QVector<Variable*> Model::getAllResultParameters()
{
    QMutexLocker locker(mutex);
    return resultvarcontainer->getAllVars();
}

int Model::numberOfCompareParameters()
{
    QMutexLocker locker(mutex);
    return calccontainer->getAllVars().size();
}

Variable * Model::getParameter(QString parametername)
{
    QMutexLocker locker(mutex);
    if(parametername=="")
        qFatal("Model::getParameter(): Parametername is empty");

    if(!existsParameter(parametername))
        qFatal("Model::getParameter(): Parameter does not exist");

    if(inputvarcontainer->contains(parametername))
        return inputvarcontainer->getVar(parametername);

    if(resultvarcontainer->contains(parametername))
        return resultvarcontainer->getVar(parametername);

    if(iterationvarcontainer->contains(parametername))
        return iterationvarcontainer->getVar(parametername);

    if(calccontainer->contains(parametername))
        return calccontainer->getVar(parametername);

    qFatal("Model::getParameter(): ERROR this code section should never be reached");

    return NULL;
}

CalculationVariable * Model::getCompareParameter(int index)
{
    QMutexLocker locker(mutex);
    return calccontainer->getAllVars().at(index);
}

QVector<CalculationVariable*> Model::getAllCompareParameters()
{
    QMutexLocker locker(mutex);
    return calccontainer->getAllVars();
}

QStringList Model::getNotEvaluatingGroups()
{
    QMutexLocker locker(mutex);
    return QStringList(*notevaluatinggroups);
}

QStringList Model::getEvaluatingGroups()
{
    QMutexLocker locker(mutex);
    return QStringList(*evaluatinggroups);
}

QVector<CalibrationVariable*> Model::getEvaluatingParameters()
{
    qDebug("Model::getEvaluatingParameters()");
    QMutexLocker locker(mutex);
    QVector<CalibrationVariable*> result;

    for(int counter=0; counter<evaluatinggroups->size(); counter++)
    {
        QCoreApplication::processEvents();
        QVector<CalibrationVariable*> *tmpgroup = groups->at(groupnames->indexOf(evaluatinggroups->at(counter)));

        for(int counter2=0; counter2<tmpgroup->size(); counter2++)
        {
            QCoreApplication::processEvents();
            if(!result.contains(tmpgroup->at(counter2)))
                result.append(tmpgroup->at(counter2));
        }
    }

    for(int counter=0; counter<notevaluatinggroups->size(); counter++)
    {
        QCoreApplication::processEvents();
        int index = groupnames->indexOf(notevaluatinggroups->at(counter));

        if(index<0 || index>groups->size()-1)
            qFatal("Model::getEvaluatingParameters(): Wrong Index: %d",index) ;

        QVector<CalibrationVariable*> *tmpgroup = groups->at(groupnames->indexOf(notevaluatinggroups->at(counter)));

        for(int counter2=0; counter2<tmpgroup->size(); counter2++)
        {
            QCoreApplication::processEvents();
            if(result.contains(tmpgroup->at(counter2)))
            {
                result.remove(result.indexOf(tmpgroup->at(counter2)));
            }
        }
    }
    qDebug("Model::getEvaluatingParameters() DONE");
    return result;
}

bool Model::deleteParameterFromAllGroups(QString parametername)
{
    QMutexLocker locker(mutex);
    qDebug("Model::deleteParameterFromAllGroups()");

    if(parameterevaluator->isFreeParameterName(parametername))
        return false;

    for(int counter=0; counter<groups->size(); counter++)
    {
        QVector<CalibrationVariable*> * tmpgroup = groups->at(counter);
        if(tmpgroup->contains(static_cast<CalibrationVariable*>(getParameter(parametername))))
        {

            removeParameterFromGroup(groupnames->at(counter),parametername);

        }
    }

    emit modelchanged(this);
    return true;
}

bool Model::deleteGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    qDebug("Model::deleteGroup()");

    if(groupname=="default")
        return false;

    if(!groupnames->contains(groupname))
        return false;

    if(groupname==groupnames->at(0))
        return false;

    int index = groupnames->indexOf(groupname);
    if(evaluatinggroups->contains(groupname))
        removeEvaluatingGroup(groupname);

    if(notevaluatinggroups->contains(groupname))
        removeNotEvaluatingGroup(groupname);

    groupnames->removeAt(index);
    delete (groups->at(index));
    groups->remove(index);

    emit modelchanged(this);
    return true;
}

bool Model::newGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    qDebug("Model::newGroup()");
    if(groupnames->contains(groupname))
        return false;

    groupnames->append(groupname);
    groups->append(new QVector<CalibrationVariable*>());

    emit modelchanged(this);
    return true;
}

bool Model::removeParameterFromGroup(QString groupname, QString parametername)
{
    QMutexLocker locker(mutex);
    qDebug("Model::removeParameterFromGroup()");
    if(groupname==groupnames->at(0))
        return false;

    if(!existsGroup(groupname))
        return false;

    if(parameterevaluator->isFreeParameterName(parametername))
        return false;

    QVector<CalibrationVariable*> *tmpgroup = groups->at(groupnames->indexOf(groupname));

    if(tmpgroup->contains(dynamic_cast<CalibrationVariable*>(getParameter(parametername))))
    {
        tmpgroup->remove(tmpgroup->indexOf(dynamic_cast<CalibrationVariable*>(getParameter(parametername))));
    }

    emit modelchanged(this);
    return true;
}

bool Model::addParameterToGroup(QString groupname, CalibrationVariable* parameter)
{
    QMutexLocker locker(mutex);
    qDebug("Model::addParameterToGroup()");
    if(parameterevaluator->isFreeParameterName(parameter->getName()))
        return false;

    if(!groupnames->contains(groupname))
        return false;

    if(groups->at(groupnames->indexOf(groupname))->contains(parameter))
    {
        return false;
    }


    groups->at(groupnames->indexOf(groupname))->append(parameter);

    emit modelchanged(this);
    return true;
}

QVector<CalibrationVariable*> Model::getGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    return *(groups->at(groupnames->indexOf(groupname)));
}

QStringList Model::getGroupNames()
{
    QMutexLocker locker(mutex);
    return QStringList(*groupnames);
}

bool Model::existsGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    if(groupnames->contains(groupname))
        return true;

    return false;
}

bool Model::removeNotEvaluatingGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    if(!groupnames->contains(groupname))
        return false;

    if(!notevaluatinggroups->contains(groupname))
        return false;

    notevaluatinggroups->removeAt(notevaluatinggroups->indexOf(groupname));

    emit modelchanged(this);

    return true;
}

bool Model::removeEvaluatingGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    if(groupname==groupnames->at(0))
        return false;

    if(!groupnames->contains(groupname))
        return false;

    if(!evaluatinggroups->contains(groupname))
        return false;

    evaluatinggroups->removeAt(evaluatinggroups->indexOf(groupname));

    if(evaluatinggroups->size()==0)
        evaluatinggroups->append(groupnames->at(0));

    emit modelchanged(this);

    return true;
}

bool Model::addNotEvaluatingGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    if(!groupnames->contains(groupname))
        return false;

    if(notevaluatinggroups->contains(groupname))
        return false;

    notevaluatinggroups->append(groupname);

    emit modelchanged(this);
    return true;
}

bool Model::addEvaluatingGroup(QString groupname)
{
    QMutexLocker locker(mutex);
    if(groupname==groupnames->at(0))
        return false;

    if(!groupnames->contains(groupname))
        return false;

    if(evaluatinggroups->contains(groupname))
        return false;

    if(evaluatinggroups->at(0)==groupnames->at(0))
        evaluatinggroups->removeAt(0);

    evaluatinggroups->append(groupname);

    emit modelchanged(this);
    return true;
}

bool Model::existsParameter(QString parametername)
{
    QMutexLocker locker(mutex);
    return !parameterevaluator->isFreeParameterName(parametername);
}

void Model::setAlg(QString name)
{
    QMutexLocker locker(mutex);
    qDebug("Model::setAlg()");
    if(name!="")
    {
        algname=name;
        emit modelchanged(this);
    }
}

QString Model::getAlg()
{
    QMutexLocker locker(mutex);
    return algname;
}


QScriptEngine* Model::getScriptEngine()
{
    QMutexLocker locker(mutex);
    qDebug("Model::getScriptEngine()");
    QScriptEngine *engine = new QScriptEngine();
    qScriptRegisterMetaType(engine, ScriptLibary::calculationVariableToScriptValue, ScriptLibary::calculationVariableFromScriptValue);
    qScriptRegisterMetaType(engine, ScriptLibary::iterationResultToScriptValue, ScriptLibary::iterationResultFromScriptValue);
    qScriptRegisterMetaType(engine, ScriptLibary::calibrationVariableToScriptValue, ScriptLibary::calibrationVariableFromScriptValue);
    qScriptRegisterSequenceMetaType<QVector<IterationResult*> >(engine);
    qScriptRegisterSequenceMetaType<QVector<double> >(engine);

    engine->globalObject().setProperty("calibrationparameter", qScriptValueFromSequence(engine,this->getEvaluatingParameters()));
    engine->globalObject().setProperty("compareparameter", qScriptValueFromSequence(engine,this->getAlgParameters()));

    QScriptValue objectValue = engine->newQObject(coreengine);
    engine->globalObject().setProperty("engine", objectValue);
    QScriptValue print = engine->newFunction(ScriptLibary::printLog);
    engine->globalObject().setProperty("print", print);
    QScriptValue include = engine->newFunction(ScriptLibary::include);
    include.setProperty("WORKINGDIR", ScriptLibary::getInstance()->getCalibrationScriptPath()  + algname);
    engine->globalObject().setProperty("include", include);
    qDebug("Model::getScriptEngine()...DONE");
    return engine;
}

void Model::setMaxIterations(int iterations)
{
    QMutexLocker locker(mutex);
    if(iterations<0)
        return;

    if(iterations>5000)
        iterations=5000;

    maxiterations=iterations;

    emit modelchanged(this);
}

int Model::getMaxIterations()
{
    QMutexLocker locker(mutex);
    return maxiterations;
}

bool Model::getClearIterations()
{
    QMutexLocker locker(mutex);
    return cleariterations;
}

void Model::setClearIterations(bool clear)
{
    QMutexLocker locker(mutex);
    qDebug("Model::setClearIterations()");
    if(cleariterations==clear)
        return;

    cleariterations=clear;
    emit modelchanged(this);
}

bool Model::updateIterationParamters(int index)
{
    QMutexLocker locker(mutex);
    qDebug("Model::updateIterationParamters()");

    for(int counter=0; counter<numberOfIterationFiles(); counter++)
    {
        QString values = "";
        QString iterationfilename = getIterationFileName(counter);

        if(!Persistence::loadFileAsString(iterationfilename.replace(QRegExp("\\$iteration\\$"), QString::number(index)),&values))
            return false;

        for(int counter2=0; counter2<numberOfIterationParameters(getIterationFileName(counter)); counter2++)
        {
            if(!parameterevaluator->getCurrentValueOfParameter(getIterationParameter(counter2, getIterationFileName(counter)),getIterationTemplate(getIterationFileName(counter)),values))
                return false;
        }
    }


    return true;
}

bool Model::createExternalInputFile(QString inputfilename, QString outputfilename, int index)
{
    QMutexLocker locker(mutex);
    qDebug("Model::createExternalInputFile()");
    bool containsiteration = false;
    QString result = "";

    if(numberOfInputParameters(inputfilename)==0)
    {
        result=getInputTemplate(inputfilename);
    }
    else
    {
        QStringList stringlist = getInputTemplate(inputfilename).split("\n");

        for(int counter=0; counter<stringlist.size(); counter++)
        {
            QString line = stringlist.at(counter);
            CalibrationVariable *var = NULL;

            while(line.contains("$"))
            {
                QStringList tokens = line.split("$");

                var = NULL;

                for(int tokenindex=0; tokenindex<tokens.size(); tokenindex++)
                {
                    for(int varindex=0; varindex<numberOfInputParameters(inputfilename); varindex++)
                    {
                        if(tokens.at(tokenindex).contains(QRegExp("^" + getInputParameter(varindex,inputfilename)->getName() + "(_\\d+)?$")))
                        {
                            var = getInputParameter(varindex,inputfilename);
                            break;
                        }
                    }

                    if(var!=NULL)
                    {

                        int parameterindex = 0;

                        if(tokens.at(tokenindex).contains(var->getName()))
                        {
                            if(tokens.at(tokenindex).contains("_"))
                            {
                                QStringList parametersplit = tokens.at(tokenindex).split("_");

                                if(parametersplit.size()<2)
                                    qFatal("Model::createExternalInputFile(): ERROR in Templatefile (No Vector)");

                                bool indexok = false;

                                parameterindex = parametersplit.at(1).toInt(&indexok);

                                if(!indexok)
                                    qFatal("Model::createExternalInputFile(): ERROR in Templatefile (Wrong index)");
                            }
                        }

                        if(parameterindex>=var->getValues().size())
                            qFatal("Model::createExternalInputFile(): ERROR in Templatefile (No Vector index)");

                        if(parameterindex==0)
                        {
                            line.replace(QRegExp("\\$" + var->getName() + "\\$"), QString::number(var->getValues().at(parameterindex)));
                        }
                        else
                        {
                            line.replace(QRegExp("\\$" + var->getName() + QString("_") + QString::number(parameterindex) + "\\$"), QString::number(var->getValues().at(parameterindex)));
                        }
                    }
                }
            }

            result+=line;

            if(counter<stringlist.size()-1)
                result+="\n";
        }
    }

    if(inputfilename.contains(QRegExp("\\$iteration\\$")))
        containsiteration = true;


    return Persistence::saveStringAsFile(outputfilename.replace(QRegExp("\\$iteration\\$"), QString::number(index)), result);
}

bool Model::createExternalFiles(int index)
{
    QMutexLocker locker(mutex);
    bool dirty = true;
    for(int counter=0; counter<numberOfInputFiles(); counter++)
    {
        if(!createExternalInputFile(getInputFileName(counter),getInputFileName(counter),index))
            dirty=false;
    }
    return dirty;
}

void Model::saveIteration(int index, bool saveall)
{
    QMutexLocker locker(mutex);
    iterationresults->append(new IterationResult(this,index,saveall));
}

IterationResult * Model::getIterationResult(int index, bool includenotfinished)
{
    QMutexLocker locker(mutex);
    if(index+1>numberOfCompleteIterations() && !includenotfinished)
        qFatal("Model::getIterationResult(): NOT ALLOWED: Do not try to enter a \"not complete\" IterationResult");

    if(index<0)
        qFatal("Model::getIterationResult(): -1 no valid index");

    return iterationresults->at(index);
}

void Model::clearIterationResults()
{
    qDebug("Model::clearIterationResults()");
    QMutexLocker locker(mutex);

    while(iterationresults->size()>0)
    {
        delete iterationresults->at(0);
        iterationresults->remove(0);
    }

    for(int counter=0; counter<getAllCalibrationParameters().size(); counter++)
    {
        CalibrationVariable *tmp = getAllCalibrationParameters().at(counter);
        tmp->setValues(tmp->getInitValue());
    }
}

int Model::numberOfIterations()
{
    QMutexLocker locker(mutex);
    return iterationresults->size();
}

void Model::setIterationResults(QVector<IterationResult*> *results)
{
    qDebug("Model::setIterationResults()");
    QMutexLocker locker(mutex);
    clearIterationResults();
    delete iterationresults;
    iterationresults=results;
}

bool Model::changeInputFileName(QString oldname, QString newname)
{
    QMutexLocker locker(mutex);
    return inputvarcontainer->renameTemplateName(oldname,newname);
}

bool Model::changeResultFileName(QString oldname, QString newname)
{
    QMutexLocker locker(mutex);
    return resultvarcontainer->renameTemplateName(oldname,newname);
}

bool Model::changeIterationFileName(QString oldname, QString newname)
{
    QMutexLocker locker(mutex);
    return iterationvarcontainer->renameTemplateName(oldname,newname);
}

QString Model::getPreExec()
{
    QMutexLocker locker(mutex);
    return preexec;
}

QString Model::getPreExecArgument()
{
    QMutexLocker locker(mutex);
    return preexecargument;
}

QString Model::getIterationExec()
{
    QMutexLocker locker(mutex);
    return iterationexec;
}

QString Model::getIterationExecArgument()
{
    QMutexLocker locker(mutex);
    return iterationexecargument;
}

QString Model::getPostExec()
{
    QMutexLocker locker(mutex);
    return postexec;
}

QString Model::getPostExecArgument()
{
    QMutexLocker locker(mutex);
    return postexecargument;
}

QString Model::getIterationWorkspace()
{
    QMutexLocker locker(mutex);
    return iterationworkspace;
}

QString Model::getPostWorkspace()
{
    QMutexLocker locker(mutex);
    return postworkspace;
}

QString Model::getPreWorkspace()
{
    QMutexLocker locker(mutex);
    return preworkspace;
}

void Model::setPreExec(QString string)
{
    QMutexLocker locker(mutex);
    preexec=string;
    emit modelchanged(this);
}

void Model::setPreExecArgument(QString string)
{
    QMutexLocker locker(mutex);
    preexecargument=string;
    emit modelchanged(this);
}

void Model::setIterationExec(QString string)
{
    QMutexLocker locker(mutex);
    iterationexec=string;
    emit modelchanged(this);
}

void Model::setIterationExecArgument(QString string)
{
    QMutexLocker locker(mutex);
    iterationexecargument=string;
    emit modelchanged(this);
}

void Model::setPostExec(QString string)
{
    QMutexLocker locker(mutex);
    postexec=string;
    emit modelchanged(this);
}

void Model::setPostExecArgument(QString string)
{
    QMutexLocker locker(mutex);

    postexecargument=string;
    emit modelchanged(this);
}

void Model::setPreWorkspace(QString string)
{
    QMutexLocker locker(mutex);

    preworkspace=string;
    emit modelchanged(this);
}

void Model::setPostWorkspace(QString string)
{
    QMutexLocker locker(mutex);

    postworkspace=string;
    emit modelchanged(this);
}

void Model::setIterationWorkspace(QString string)
{
    QMutexLocker locker(mutex);

    iterationworkspace=string;
    emit modelchanged(this);
}

int Model::getCpus()
{
    QMutexLocker locker(mutex);
    return cpunum;
}

void Model::setCpus(int cpus)
{
    QMutexLocker locker(mutex);
    if(cpus<1||cpus>99)
        return;

    cpunum=cpus;
    emit modelchanged(this);
}

int Model::numberOfCompleteIterations()
{
    QMutexLocker locker(mutex);
    int result=0;

    for(int counter=0; counter<iterationresults->size(); counter++)
    {
        if(iterationresults->at(counter)->isComplete())
        {
            result++;
        }
        else
        {
            return result;
        }
    }

    return result;
}

bool Model::updateIterationResult(int index)
{
    QMutexLocker locker(mutex);
    if(!updateIterationParamters(index))
        return false;

    IterationResult *result = getIterationResult(index,true);
    result->updateIterationParameters(this);

    return true;
}

QVector<IterationResult*> Model::getCompleteIterationResults()
{
    QMutexLocker locker(mutex);
    QVector<IterationResult*> result;
    for(int counter=0; counter<this->numberOfCompleteIterations(); counter++)
    {
        result.append(this->getIterationResult(counter,false));
    }

    return result;
}

void Model::setCoreEngine(CoreEngine *engine)
{
    QMutexLocker locker(mutex);
    if(engine==NULL)
        qFatal("Model::setCoreEngine(): CoreEngine is a null pointer");

    coreengine=engine;
}

CoreEngine* Model::getCoreEngine()
{
    QMutexLocker locker(mutex);
    return coreengine;
}

QVector<CalculationVariable*> Model::getAlgParameters()
{
    QMutexLocker locker(mutex);
    qDebug("Model::getAlgParameters()");
    return *algparameters;
}

bool Model::addCalculationVariableToCalibration(QString name)
{
    qDebug("Model::addCalculationVariableToCalibration()");
    QMutexLocker locker(mutex);
    if(name=="")
        return false;

    CalculationVariable * var = static_cast<CalculationVariable*>(getParameter(name));
    if(algparameters->contains(var))
        return false;

    algparameters->append(var);
    emit modelchanged(this);
    return true;
}

bool Model::removeCalculationVariableFromCalibration(QString name)
{
    qDebug("Model::removeCalculationVariableFromCalibration()");
    QMutexLocker locker(mutex);
    if(name=="")
        return false;

    CalculationVariable * var = static_cast<CalculationVariable*>(getParameter(name));
    if(!algparameters->contains(var))
        return false;

    algparameters->remove(algparameters->indexOf(var));
    emit modelchanged(this);
    return true;
}

bool Model::exportIteration(int iteration, QString directory)
{
    QMutexLocker locker(mutex);
    if(!loadIteration(iteration))
        return false;

    for(int counter=0; counter<numberOfInputFiles(); counter++)
    {
        QStringList filenamelist = getInputFileName(counter).split(QRegExp("[/ | \\\\]"));
        QString filename = filenamelist.last();

        if(!createExternalInputFile(getInputFileName(counter),QString(directory + "/" + filename),iteration+1))
            return false;
    }
    return true;
}

int Model::getBestIteration(double *value)
{
    QMutexLocker locker(mutex);
    QVector<double> bestiteration;

    int maxiterations = numberOfCompleteIterations();
    QVector<CalculationVariable*> parameters = getAlgParameters();
    QVector<IterationResult*> iterationresults = getCompleteIterationResults();

    for(int iteration=0; iteration<maxiterations; iteration++)
    {
        QCoreApplication::processEvents();
        double result = 0;

        for(int index=0; index<parameters.size(); index++)
        {
            QCoreApplication::processEvents();
            QString name=parameters.at(index)->getName();

            for(int counter=0; counter<iterationresults.at(iteration)->getValueOfParameter(name).size(); counter++)
                result+=iterationresults.at(iteration)->getValueOfParameter(name).at(counter);
        }

        result/=parameters.size();

        bestiteration.append(result);
    }

    double currentmin = 0;
    int currentindex=0;

    for(int index=0; index<bestiteration.size(); index++)
    {   
        if(qAbs(currentmin)>qAbs(bestiteration.at(index)) || index==0)
        {
            currentmin=bestiteration.at(index);
            currentindex=index+1;
        }
    }

    if(value!=NULL)
        *value=currentmin;

    return currentindex;
}

bool Model::loadIteration(int index)
{
    QMutexLocker locker(mutex);

    if(index+1>numberOfCompleteIterations() || index<0)
        return false;

    IterationResult *result = getIterationResult(index,false);

    QVector<CalibrationVariable*> calibrationvar = inputvarcontainer->getAllVars();
    for(int counter=0; counter<calibrationvar.size(); counter++)
        calibrationvar.at(counter)->setValues(result->getValueOfInputParameter(calibrationvar.at(counter)->getName()));

    QVector<Variable*> iterationvar = iterationvarcontainer->getAllVars();
    for(int counter=0; counter<iterationvar.size(); counter++)
        iterationvar.at(counter)->setValues(result->getValueOfInputParameter(iterationvar.at(counter)->getName()));

    QVector<Variable*> resultvar = resultvarcontainer->getAllVars();
    for(int counter=0; counter<resultvar.size(); counter++)
        resultvar.at(counter)->setValues(result->getValueOfInputParameter(resultvar.at(counter)->getName()));

    return true;
}

QMap<QString,QVector<QVector<double> > > Model::getEvaluatingParametersResult()
{
    QMutexLocker locker(mutex);
    qDebug() << "Model::getEvaluatingParametersResult()" << QTime::currentTime();
    QMap<QString,QVector<QVector<double> > > list;
    int maxiterations = numberOfCompleteIterations();
    QVector<CalibrationVariable*> parameters = getEvaluatingParameters();
    QVector<IterationResult*> iterationresults = getCompleteIterationResults();

    for(int index=0; index<parameters.size(); index++)
    {
        QString name=parameters.at(index)->getName();
        QVector<QVector<double> > results;

        for(int iteration=0; iteration<maxiterations; iteration++)
        {
            results.append(iterationresults.at(iteration)->getValueOfParameter(name));
        }
        list.insert(name,results);
    }

    qDebug() << "Model::getEvaluatingParametersResult() DONE" << QTime::currentTime();
    return list;
}

QMap<QString,QVector<QVector<double> > > Model::getAlgParametersResult()
{
    QMutexLocker locker(mutex);
    qDebug() << "Model::getAlgParametersResult() " << QTime::currentTime();
    QMap<QString,QVector<QVector<double> > > list;
    int maxiterations = numberOfCompleteIterations();
    QVector<CalculationVariable*> parameters = getAlgParameters();
    QVector<IterationResult*> iterationresults = getCompleteIterationResults();

    for(int index=0; index<parameters.size(); index++)
    {
        QString name=parameters.at(index)->getName();
        QVector<QVector<double> > results;

        for(int iteration=0; iteration<maxiterations; iteration++)
        {
            results.append(iterationresults.at(iteration)->getValueOfParameter(name));
        }

        list.insert(name,results);
    }
    qDebug() << "Model::getAlgParametersResult() DONE" << QTime::currentTime();
    return list;
}

bool Model::modelLocked()
{
    if(mutex->tryLock(0))
    {
        mutex->unlock();
        return false;
    }

    return true;
}

bool Model::setMaxError(double error)
{
    maxerror=error;
    emit modelchanged(this);
    return true;
}

double Model::getMaxError()
{
    return maxerror;
}
