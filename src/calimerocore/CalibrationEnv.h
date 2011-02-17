#ifndef CALIBRATIONENV_H
#define CALIBRATIONENV_H

#include <Registry.h>
#include <IObjectiveFunction.h>
#include <IModelSimulator.h>
#include <ICalibrationAlg.h>
#include <Calibration.h>
#include <QThread>
#include <CalimeroGlob.h>
#include <ModelSimThreadPool.h>
#include <ExternalParameterRegistry.h>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <IResultHandler.h>

class QWidget;

class CALIMERO_PUBLIC CalibrationEnv : public QThread
{
public:
    enum CALIBRATIONSTATE{CALIBRATIONINIT=0, CALIBRATIONRUNNING=1, CALIBRATIONSHUTDOWN=2, CALIBRATIONNOTRUNNING=3};

private:
    int numthread;
    bool inmemory;
    bool stopthread;
    static CalibrationEnv *instance;
    QMutex *mutex;
    QReadWriteLock lock;
    Calibration *calibration;
    ModelSimThreadPool *threadpool;
    Registry<IObjectiveFunction>* oreg;
    Registry<IModelSimulator>* mreg;
    Registry<ICalibrationAlg>* creg;
    Registry<IResultHandler>* rreg;
    CALIBRATIONSTATE calstate;

    void runCalibration();
    CALIBRATIONSTATE getCalibrationState();
    void setCalibrationState(CALIBRATIONSTATE state);

public:
    CalibrationEnv();
    ~CalibrationEnv();
    void run();
    static CalibrationEnv* getInstance();
    Registry<IObjectiveFunction>* getObjectiveFunctionReg();
    Registry<IModelSimulator>* getModelSimulatorReg();
    Registry<ICalibrationAlg>* getCalibrationAlgReg();
    Registry<IResultHandler>* getResultHandlerReg();
    vector<string> getAvailableObjectiveFunctions();
    vector<string> getAvailableModelSimulators();
    vector<string> getAvailableCalibrationAlgs();
    vector<string> getAvailableResultHandlers();

    //calibration-settings
    bool setCalibration(Calibration *cal);
    Calibration* getCalibration();
    bool isCalibrationRunning();
    bool calibrationShutDownState();
    int getNumThreads();
    bool setNumThreads(int num);
    bool inMemory();
    void setInMemory(bool inmemory);

    //calibration
    bool startCalibration();
    void stopCalibration();
    bool execIteration(vector<CalibrationVariable*> calibrationparameters);
    void barrier();

    //result analysis
    void runAllEnabledResultHandler();
    void runResultHandler(string name);
};

#endif // CALIBRATIONENV_H
