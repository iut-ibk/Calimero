#ifndef CALIBRATIONENV_H
#define CALIBRATIONENV_H

#include <Registry.h>
#include <IObjectiveFunction.h>
#include <IModelSimulator.h>
#include <ICalibrationAlg.h>
#include <Calibration.h>
#include <QThread>

class CalibrationEnv : public QThread
{
public:
    enum CALIBRATIONSTATE{CALIBRATIONINIT=0, CALIBRATIONRUNNING=1, CALIBRATIONSHUTDOWN=2, CALIBRATIONNOTRUNNING=3};
private:
    bool stopthread;
    static CalibrationEnv *instance;
    Calibration *calibration;
    Registry<IObjectiveFunction>* oreg;
    Registry<IModelSimulator>* mreg;
    Registry<ICalibrationAlg>* creg;
    CALIBRATIONSTATE calstate;
    CalibrationEnv();
    void runCalibration();

public:
    ~CalibrationEnv();
    void run();
    static CalibrationEnv* getInstance();
    Registry<IObjectiveFunction>* getObjectiveFunctionReg();
    Registry<IModelSimulator>* getModelSimulatorReg();
    Registry<ICalibrationAlg>* getCalibrationAlgReg();

    //calibration
    bool setCalibration(Calibration *cal);
    Calibration* getCalibration();
    bool isCalibrationRunning();

    //calibration
    bool startCalibration();
    void stopCalibration();

private:
    Variable* cloneParameter(Variable* old);
};

#endif // CALIBRATIONENV_H
