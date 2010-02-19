#include "IterationRunnable.h"
#include "CoreEngine.h"
#include <QProcess>
#include <Model.h>
#include <QFile>
#include <QDir>
#include <QtCore>
#include <iostream>
#include "IterationResult.h"
#include <QScriptEngine>
#include "CalculationVariable.h"

IterationRunnable::IterationRunnable(int index, CoreEngine *engine, Model *model)
{
    this->index=index;
    coreengine=engine;
    this->model=model;
}

void IterationRunnable::run()
{
    qDebug() << "IterationRunnable::run()";
    process = new QProcess();
    QTime time;
    time.start();
    QString iterationworkspace = model->getIterationWorkspace();
    QString preworkspace = model->getPreWorkspace();
    QString postworkspace = model->getPostWorkspace();

    QString preprog = model->getPreExec().replace(QRegExp("\\$iteration\\$"), QString::number(index));
    QString iterationprog = model->getIterationExec().replace(QRegExp("\\$iteration\\$"), QString::number(index));
    QString postprog = model->getPostExec().replace(QRegExp("\\$iteration\\$"), QString::number(index));

    QStringList preprogarguments = model->getPreExecArgument().replace(QRegExp("\\$iteration\\$"), QString::number(index)).split(" ");
    QStringList iterationprogarguments = model->getIterationExecArgument().replace(QRegExp("\\$iteration\\$"), QString::number(index)).split(" ");
    QStringList postprogarguments = model->getPostExecArgument().replace(QRegExp("\\$iteration\\$"), QString::number(index)).split(" ");

    QDir dir(preworkspace);

    if(preprog!="" && !dir.exists(preprog))
    {
        coreengine->abortCalibration(CoreEngine::ProgramNotExist, preprog);
        delete process;
        return;
    }

    dir.setPath(iterationworkspace);

    if(!dir.exists(iterationprog))
    {
        coreengine->abortCalibration(CoreEngine::ProgramNotExist, iterationprog);
        delete process;
        return;
    }

    dir.setPath(postworkspace);

    if(postprog!="" && !dir.exists(postprog))
    {
        coreengine->abortCalibration(CoreEngine::ProgramNotExist, postprog);
        delete process;
        return;
    }

    if(preprog!="")
    {
        process->setWorkingDirectory(preworkspace);
        process->start(preprog,preprogarguments);
        process->waitForStarted();

        if(!process->waitForFinished(-1))
        {
            coreengine->abortCalibration(CoreEngine::ExternalProgramError, preprog);
            delete process;
            return;
        }
    }

    process->setWorkingDirectory(iterationworkspace);
    process->start(iterationprog,iterationprogarguments);
    process->waitForStarted();

    if(!process->waitForFinished(-1))
    {
        coreengine->abortCalibration(CoreEngine::ExternalProgramError, iterationprog);
        delete process;
        return;
    }


    if(postprog!="")
    {
        process->setWorkingDirectory(postworkspace);
        process->start(postprog,postprogarguments);
        process->waitForStarted();

        if(!process->waitForFinished(-1))
        {
            coreengine->abortCalibration(CoreEngine::ExternalProgramError, postprog);
            delete process;
            return;
        }
    }

    while(time.elapsed()<100){};

    if(!model->updateIterationResult(index))
    {
        coreengine->abortCalibration(CoreEngine::NoOutputFiles);
        delete process;
        return;
    }

    IterationResult *result = model->getIterationResult(index);
    QVector<CalculationVariable*> objectivefunctions = model->getAlgParameters();

    bool calibrationok = true;

    for(int index=0; index<objectivefunctions.size(); index++)
    {
        double objectiveresult = result->getValueOfParameter(objectivefunctions.at(index)->getName()).at(0);

        if(objectiveresult>model->getMaxError())
            calibrationok=false;
    }

    if(calibrationok)
        coreengine->abortCalibration(CoreEngine::MaxErrorReached);

    delete process;
    return;
}

