#include <CalibrationEnv.h>
#include <Logger.h>

CalibrationEnv* CalibrationEnv::instance = 0;

CalibrationEnv::CalibrationEnv()
{
    stopthread=false;
    calstate=CALIBRATIONNOTRUNNING;
    oreg = new Registry<IObjectiveFunction>();
    mreg = new  Registry<IModelSimulator>();
    creg = new Registry<ICalibrationAlg>();
    threadpool = 0;
    numthread=1;
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

int CalibrationEnv::getNumThreads()
{
    return numthread;
}

bool CalibrationEnv::setNumThreads(int num)
{
    if(calstate!=CALIBRATIONNOTRUNNING)
    {
        Logger(Error) << "Already running calibration - could not set number of threads";
        return false;
    }

    if(num < 1)
    {
        Logger(Error) << "Number of threads must be > 0";
        return false;
    }

    numthread=num;

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
            Logger(Standard) << "Calibration started";
            runCalibration();
            calstate=CALIBRATIONNOTRUNNING;
            Logger(Standard) << "Calibration stopped";
        }

        msleep(10);
    }
}

void CalibrationEnv::runCalibration()
{
    calstate=CALIBRATIONRUNNING;

    if(calibration->getCalibrationAlg()=="")
    {
        Logger(Error) << "No calibration algorithm set";
        return;
    }

    if(calibration->getModelSimulator()=="")
    {
        Logger(Error) << "No model simulator set";
        return;
    }

    ICalibrationAlg *tmpalg = creg->getFunction(calibration->getCalibrationAlg());

    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getCalibrationAlgSettings())
            tmpalg->setValueOfParameter(p.first,p.second);

    int realthreads = 0;

    if(tmpalg->containsParameter("parallel"))
        realthreads = (boost::lexical_cast<double>(tmpalg->getValueOfParameter("parallel"))) ? numthread : 1;
    else
        realthreads = 1;

    threadpool = new ModelSimThreadPool(realthreads);

    tmpalg->start();
    delete threadpool;
    delete tmpalg;
}

bool CalibrationEnv::isCalibrationRunning()
{
    return (calstate==CALIBRATIONNOTRUNNING) ? 0 : 1;
}

bool CalibrationEnv::exec(vector<CalibrationVariable*> calibrationparameters)
{
    if(calstate!=CALIBRATIONSHUTDOWN)
    {
        Logger(Error) << "Shut down running calibration";
        return false;
    }

    threadpool->pushIteration(calibrationparameters);
    return true;
}
