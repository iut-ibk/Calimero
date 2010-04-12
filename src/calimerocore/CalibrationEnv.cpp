#include <CalibrationEnv.h>
#include <Logger.h>

CalibrationEnv* CalibrationEnv::instance = 0;

CalibrationEnv::CalibrationEnv()
{
    stopthread=false;
    oreg = new Registry<IObjectiveFunction>();
    mreg = new  Registry<IModelSimulator>();
    creg = new Registry<ICalibrationAlg>();
    calstate=CALIBRATIONNOTRUNNING;
}

CalibrationEnv::~CalibrationEnv()
{
    stopthread=true;
    while(CalibrationEnv::getInstance()->isRunning())
        wait(1);
    delete oreg;
    delete mreg;
    delete creg;
    instance=0;
}

CalibrationEnv* CalibrationEnv::getInstance()
{
    if(!instance)
    {
        instance = new CalibrationEnv();
        instance->start();
    }
    return instance;
}

Registry<IObjectiveFunction>* CalibrationEnv::getObjectiveFunctionReg()
{
    if(!oreg)
        oreg = new Registry<IObjectiveFunction>();
    return oreg;
}

Registry<IModelSimulator>* CalibrationEnv::getModelSimulatorReg()
{
    if(!mreg)
        mreg = new Registry<IModelSimulator>();
    return mreg;
}

Registry<ICalibrationAlg>* CalibrationEnv::getCalibrationAlgReg()
{
    if(!creg)
        creg = new Registry<ICalibrationAlg>();
    return creg;
}

Variable* CalibrationEnv::cloneParameter(Variable* old)
{
    Variable *result;

    switch(old->getType())
    {
    case Variable::ITERATIONVARIABLE:
        result = new Variable(old->getName(),old->getValues(),old->getType());
        break;
    case Variable::CALIBRATIONVARIABLE:
        result = new CalibrationVariable(old->getName(), old->getValues());
        break;
    case Variable::OBSERVEDVARIABLE:
        result = new Variable(old->getName(),old->getValues(),old->getType());
        break;
    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        ObjectiveFunctionVariable *tmp = new ObjectiveFunctionVariable(old->getName());
        tmp->setObjectiveFunction(dynamic_cast<ObjectiveFunctionVariable*>(old)->getObjectiveFunction());
        result=tmp;
        break;
    }

    return result;
}

bool CalibrationEnv::startCalibration()
{
    if(calstate!=CALIBRATIONNOTRUNNING)
    {
        Logger(Warning) << "Calibration is already started";
        return false;
    }

    calstate=CALIBRATIONINIT;

    return true;
}

void CalibrationEnv::stopCalibration()
{
    if(calstate==CALIBRATIONNOTRUNNING)
    {
        Logger(Standard) << "No calibration running";
        return;
    }

    calstate=CALIBRATIONSHUTDOWN;
}

bool CalibrationEnv::setCalibration(Calibration *cal)
{
    if(calstate!=CALIBRATIONNOTRUNNING)
    {
        Logger(Error) << "Already running calibration";
        return false;
    }

    calibration=cal;

    return true;
}

Calibration* CalibrationEnv::getCalibration()
{
    return calibration;
}

void CalibrationEnv::run()
{
    while(!stopthread)
    {
        if(calstate==CALIBRATIONINIT)
        {
            runCalibration();
            calstate=CALIBRATIONNOTRUNNING;
        }

        sleep(1);
    }
}

void CalibrationEnv::runCalibration()
{
    calstate=CALIBRATIONNOTRUNNING;

    if(calibration->getCalibrationAlg()=="")
    {
        Logger(Error) << "No calibration algorithm set";
        return;
    }

    //TODO implement
    Logger(Debug) << "NOT IMPLEMENTED startCalibration not correct implemented yet";

    ICalibrationAlg *tmpalg = creg->getFunction(calibration->getCalibrationAlg());
    tmpalg->start();
    delete tmpalg;
}

bool CalibrationEnv::isCalibrationRunning()
{
    return (calstate==CALIBRATIONNOTRUNNING) ? 0 : 1;
}
