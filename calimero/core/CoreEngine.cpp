#include "CoreEngine.h"
#include "../persistence/Persistence.h"
#include "../qtscript/ScriptLibary.h"
#include "../core/Model.h"
#include "IterationRunnable.h"
#include <QDebug>
#include <math.h>


CoreEngine::~CoreEngine()
{
    qDebug("CoreEngine::~CoreEngine()");
    QObject::disconnect(model, SIGNAL(modelchanged(Model*)), this, SIGNAL(modelchanged(Model*)));
    QObject::disconnect(model, SIGNAL(erroroccured(QString)), this, SIGNAL(erroroccured(QString)));
    abortCalibration();
    quit();
    delete model;
}

CoreEngine::CoreEngine()
{
    qDebug("CoreEngine::CoreEngine()");
    persistencefilename="";
    currentalgorithm=NULL;
    calculating=0;
    iteration=0;
    scriptedit=false;
    nativealgorithmrunning=false;
    gui=true;
    model = new Model(this);
    threadpool=NULL;
    engine=NULL;
    exitonwarning=false;
    threadpoolabort=false;
    fileloader = new FileLoader();

    QObject::connect(model, SIGNAL(modelchanged(Model*)), this, SIGNAL(modelchanged(Model*)));
    QObject::connect(model, SIGNAL(erroroccured(QString)), this, SIGNAL(erroroccured(QString)));

    start(QThread::NormalPriority);
}

bool CoreEngine::save()
{
    qDebug() << "CoreEngine::save()";
    if(!model->modelSave(persistencefilename))
    {
        emit erroroccured(tr("Could not save file\n"));
        return false;
    }

    return true;
}

bool CoreEngine::load(QString filename, bool forcescriptupdate)
{
    qDebug() << "CoreEngine::load()";
    if(calibrationRunning())
        return false;

    emit sysInfo(QString(tr("File loading...")));
    fileloader->loadFile(filename,model,forcescriptupdate);
    int error = fileloader->fileLoadErrorCode();
    qDebug() << "CoreEngine::load()...DONE";

    if(error<1)
    {
        switch(error)
        {
        case -1: emit erroroccured(tr("Already existing script files with same name but different code\n Please load project with script import\n"));
            emit sysInfo(QString(""));
            break;

        case -2: emit erroroccured(tr("No permission to write files\n"));
            emit sysInfo(QString(""));
            break;

        default: emit erroroccured(tr("Could not load file\n"));
            emit sysInfo(QString(""));
            break;
        }

        QCoreApplication::processEvents();
        return false;
    }
    else
    {
        persistencefilename=filename;
    }

    if(!model->getClearIterations())
    {
        iteration=model->numberOfCompleteIterations();
    }
    else
    {
        iteration=0;
    }

    emit sysInfo(QString(""));
    return true;
}

Model * CoreEngine::getModel()
{
    return model;
}

bool CoreEngine::save(QString filename)
{
    persistencefilename=filename;
    return save();
}

QString CoreEngine::getPersistenceFileName()
{
    return persistencefilename;
}

void CoreEngine::startStopCalibration()
{
    qDebug("CoreEngine::startStopCalibration()");

    if(!calibrationRunning())
    {
        calculating = 1;

        while(!calibrationRunning())
            QCoreApplication::processEvents();

        emit modelchanged(model);
        return;
    }

    if(calibrationRunning())
    {
        abortCalibration();
        while(calibrationRunning())
            QCoreApplication::processEvents();
        emit modelchanged(model);
        return;
    }
}

bool CoreEngine::runIteration(bool block)
{
    qDebug("CoreEngine::runIteration()");
    QMutexLocker locker(&mutex);

    if(calculating==3)
        return false;

    if(scriptedit)
    {
        if(fakeiteration>10)
        {
            fakeiteration=0;
            engine->abortEvaluation();
        }

        fakeiteration++;
        return true;
    }

    if(model->numberOfIterations()>=model->getMaxIterations())
    {
        abortCalibration(tr("Max iterations has been reached"));
        return false;
    }

    if(model->getCpus()>1)
    {
        for(int counter=0; counter<model->numberOfInputFiles(); counter++)
        {
            if(!model->getInputFileName(counter).contains("$iteration$"))
            {
                abortCalibration(tr("Filenames have to include the string \"$iteration$\" for parallel execution"));
                return false;
            }
        }
    }

    if(!model->createExternalFiles(iteration))
    {
        abortCalibration(tr("Could not create file"));
        return false;
    }

    model->saveIteration(iteration,false);

    IterationRunnable *iterationthread = new IterationRunnable(iteration,this,model);

    while(threadpool!=NULL && !threadpool->tryStart(iterationthread))
    {
        msleep(100);
    }

    if(block)
    {
        if(threadpool->activeThreadCount()>0)
        {
            threadpool->waitForDone();
        }
    }

    if(threadpool==NULL)
        delete iterationthread;

    iteration++;

    if(!hasGui() && model->numberOfCompleteIterations()%qMax(model->getMaxIterations()/100,1)==0)
        save();

    return true;
}

void CoreEngine::run()
{
    while(true)
    {
        msleep(100);
        if(calculating==1)
        {
            qDebug("CoreEngine::run() Start calibration");
            if(!calc())
                emit erroroccured(tr("Could not start calibration"));

            clear();
            qDebug("CoreEngine::run() Calibration DONE");
        }
    }
}

bool CoreEngine::calc()
{
    qDebug("CoreEngine::calc()");

    calculating=2;
    scriptedit=false;

    if(model->getClearIterations())
    {
        model->clearIterationResults();
        iteration=0;
    }
    else
    {
        iteration=model->numberOfCompleteIterations();
    }
    
    QString fileName = ScriptLibary::getInstance()->getCalibrationScriptPath() + model->getAlg() + "/main.qs";

    if(!QFile::exists(fileName))
    {
        emit erroroccured(tr("Could not load file"));
        return false;
    }

    QFile scriptFile(fileName);
    scriptFile.open(QIODevice::ReadOnly);
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    clear();

    threadpoolabort=false;

    if(engine==NULL)
    {
        model->setCoreEngine(this);
        engine = model->getScriptEngine();
    }

    if(threadpool!=NULL)
    {
        if(threadpool->activeThreadCount()>0)
        {
            threadpool->waitForDone();
        }

        delete threadpool;
        threadpool=NULL;
    }

    if(threadpool==NULL)
    {
        threadpool = new QThreadPool();
        threadpool->setMaxThreadCount(model->getCpus());
    }

    emit sysInfo(QString(tr("Calibration running...")));
    engine->evaluate(contents);
    clear();
    emit sysInfo(QString(""));
    return true;
}

bool CoreEngine::calibrationRunning()
{
    if(engine==NULL)
        return false;
    return engine->isEvaluating();
}

void CoreEngine::abortCalibration(QString string, bool fromthreadpool)
{
    Q_UNUSED(fromthreadpool);
    QMutexLocker locker(&abortmutex);
    qDebug() << "CoreEngine::abortCalibration()";
    calculating=3;

    if(engine!=NULL)
        engine->abortEvaluation();

    if(string!="")
    {
        qDebug() << string;
        emit erroroccured(string);
    }

    emit modelchanged(model);
    qDebug() << "CoreEngine::abortCalibration() DONE";
}

QVector<IterationResult*> CoreEngine::result()
{
    return model->getCompleteIterationResults();
}

QScriptEngine* CoreEngine::getScriptEngine()
{
    qDebug() << "CoreEngine::getScriptEngine()";
    if(calculating!=0)
        return NULL;
    scriptedit=true;
    fakeiteration=0;
    engine = model->getScriptEngine();
    qDebug() << "CoreEngine::getScriptEngine() DONE";
    return engine;
}

QVector<CalculationVariable*> CoreEngine::compareParameter()
{
    return model->getAlgParameters();
}

QVector<CalibrationVariable*> CoreEngine::calibrationParamter()
{
    return model->getEvaluatingParameters();
}

bool CoreEngine::hasGui()
{
    return gui;
}

void CoreEngine::enabledGui(bool hasgui)
{
    if(gui==hasgui)
        return;

    if(!hasgui)
    {
        QObject::connect(this, SIGNAL(erroroccured(QString)),this, SLOT(warning(QString)));
        QObject::connect(this, SIGNAL(sysInfo(QString)),this, SLOT(info(QString)));
    }

    if(hasgui)
    {
        QObject::disconnect(this, SIGNAL(erroroccured(QString)),this, SLOT(warning(QString)));
        QObject::disconnect(this, SIGNAL(sysInfo(QString)),this, SLOT(info(QString)));
    }

    gui=hasgui;
}

void CoreEngine::setExitOnWarning(bool exitbool)
{
    exitonwarning = exitbool;
}

void CoreEngine::warning(QString w)
{
    qCritical() << w;
    this->save();
    if(exitonwarning)
        QCoreApplication::exit(0);
}

void CoreEngine::info(QString info)
{
    qDebug() << info;
}

void CoreEngine::abortCalibration(ErrorCode type, QString filename)
{

    if(type==ProgramNotExist)
        emit erroroccured(tr("Program ") + filename + tr(" does not exist"));

    if(type==ExternalProgramError)
        emit erroroccured(tr("Error in program: ") + filename);

    if(type==NoOutputFiles)
        emit erroroccured(tr("External program did not generate output files"));

    if(type==MaxErrorReached)
        emit erroroccured(tr("Max Error reached"));

    abortCalibration("",true);
    return;
}

bool CoreEngine::runNativeIteration(QString algorithmname)
{
    qDebug() << "CoreEngine::runNativIteration()";

    if(scriptedit)
    {
        runIteration();
        return true;
    }

    if(!nativealgorithmrunning)
    {
        qDebug() << "CoreEngine::runNativIteration() INIT of " << algorithmname;
        if(currentalgorithm!=0)
            delete currentalgorithm;

        if(!Algorithms::getAlgorithmNames().contains(algorithmname)) return false;

        currentalgorithm = Algorithms::getAlgorithmInstance(algorithmname, this);
        currentalgorithm->init(model->getEvaluatingParameters());
        nativealgorithmrunning=true;
        qDebug() << "CoreEngine::runNativIteration() INIT DONE";
    }

    //run nativeiteration
    bool ok = true;

    int iterationnumber = currentalgorithm->runIteration(model->getEvaluatingParameters(),&ok);

    if(iterationnumber>=0 && ok)
    {
        qDebug() << "CoreEngine::runLMIteration() Start calibration";
        //run external program
        if(!runIteration(true))return false;
        if(!nativealgorithmrunning)return false;

        //write result from external program back to lmalg
        qDebug() << "CoreEngine::runLMIteration() write back";
        QVector<double> objectivefunctionresults;
        objectivefunctionresults.append(model->getAlgParameters().at(0)->getCurrentValue());
        if(!currentalgorithm->setIterationResult(objectivefunctionresults, iterationnumber))
        {
            qWarning() << "CoreEngine::runNativeIteration() Error during setting objectivefunction result";
            return false;
        }
        qDebug() << "CoreEngine::runNativeIteration() DONE";
        return true;
    }

    if(!ok)
        qWarning() << "CoreEngine::runNativeIteration() Error during calibration";
    return false;
}

void CoreEngine::clear()
{
    qDebug() << "CoreEngine::clear()";

    if(nativealgorithmrunning)
    {
        nativealgorithmrunning=false;
    }

    if(engine!=NULL)
    {
        delete engine;
        engine=NULL;
    }
    
    qDebug("CoreEngine::clear(): Waiting for all external calculations");
    
    if(threadpool!=NULL)
    {
        if(threadpool->activeThreadCount()>0)
        {
            threadpool->waitForDone();
        }
        
        delete threadpool;
        threadpool=NULL;
    }

    qDebug("CoreEngine::clear(): Waiting...DONE");
    
    calculating=0;
    emit modelchanged(model);
    qDebug() << "CoreEngine::clear() DONE";
}
