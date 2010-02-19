#ifndef COREENGINE_H_INCLUDED
#define COREENGINE_H_INCLUDED

#include <QtScript>
#include <QThread>
#include <QThreadPool>
#include <QMutex>
#include "IterationResult.h"
#include "FileLoader.h"
#include "../algs/Algorithms.h"
#include "../algs/Algorithm.h"


class Model;
class Variable;
class CalibrationVariable;
class CalculationVariable;



class CoreEngine : public QThread
{
    Q_OBJECT
    private:
        Algorithm *currentalgorithm;
        QString persistencefilename;
        QProcess *externalprogram;
        FileLoader *fileloader;
        QThreadPool *threadpool;
        Model* model;
        QMutex mutex, abortmutex;
        QVector<double> sensitivity;
        int calculating;
        int iteration;
        int fakeiteration;
        int sensitivitycounter;
        bool dirtyparameter;
        bool scriptedit, gui, exitonwarning, threadpoolabort, nativealgorithmrunning;


    private:
        void clear();

    public:
        enum ErrorCode {ExternalProgramError=1, ProgramNotExist=2, NoOutputFiles=3, MaxErrorReached=4};
        CoreEngine();
        ~CoreEngine();
        bool save();
        bool save(QString filename);
        bool load(QString filename, bool forcescriptupdate=false);
        bool hasGui();
        void setExitOnWarning(bool exit);
        void enabledGui(bool hasgui);
        Model* getModel();
        QString getPersistenceFileName();
        QScriptEngine *engine;
        QScriptEngine* getScriptEngine();

    signals:
        void erroroccured(QString message);
        void modelchanged(Model* m);
        void sysInfo(QString info);

    public slots:
        bool runNativeIteration(QString algorithmname);
        void warning(QString w);
        void info(QString info);
        void startStopCalibration();
        void run();
        QVector<IterationResult*> result();
        void abortCalibration(QString string="", bool fromthreadpool=false);
        void abortCalibration(ErrorCode type, QString filename="");
        bool runIteration(bool block=false);
        bool calibrationRunning();
        bool calc();
        QVector<CalculationVariable*> compareParameter();
        QVector<CalibrationVariable*> calibrationParamter();
};

#endif // COREENGINE_H_INCLUDED
