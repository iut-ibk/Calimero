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

class CALIMERO_PUBLIC CalibrationEnv : public QThread
{
public:
    enum CALIBRATIONSTATE{CALIBRATIONINIT=0, CALIBRATIONRUNNING=1, CALIBRATIONSHUTDOWN=2, CALIBRATIONNOTRUNNING=3};
private:
    int numthread;
    bool stopthread;
    static CalibrationEnv *instance;
    Calibration *calibration;
    ModelSimThreadPool *threadpool;
    Registry<IObjectiveFunction>* oreg;
    Registry<IModelSimulator>* mreg;
    Registry<ICalibrationAlg>* creg;
    CALIBRATIONSTATE calstate;

    CalibrationEnv();
    void runCalibration();
    Variable* cloneParameter(Variable* old);

public:
    ~CalibrationEnv();
    void run();
    static CalibrationEnv* getInstance();
    Registry<IObjectiveFunction>* getObjectiveFunctionReg();
    Registry<IModelSimulator>* getModelSimulatorReg();
    Registry<ICalibrationAlg>* getCalibrationAlgReg();

    //calibration-settings
    bool setCalibration(Calibration *cal);
    Calibration* getCalibration();
    bool isCalibrationRunning();
    int getNumThreads();
    bool setNumThreads(int num);

    //calibration
    bool startCalibration();
    void stopCalibration();
    bool exec(vector<CalibrationVariable*> calibrationparameters,
              vector<Variable*> observedparameters,
              vector<Variable*> iterationparameters,
              vector<ObjectiveFunctionVariable*> objectivefunctionparameters);   
};

#endif // CALIBRATIONENV_H
