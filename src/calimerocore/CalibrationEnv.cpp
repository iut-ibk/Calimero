#include <CalibrationEnv.h>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <Domain.h>
#include <assert.h>
#include <Exception.h>

CalibrationEnv* CalibrationEnv::instance = 0;

CalibrationEnv::CalibrationEnv()
{
    stopthread=false;
    calstate=CALIBRATIONNOTRUNNING;
    oreg = new Registry<IObjectiveFunction>();
    mreg = new  Registry<IModelSimulator>();
    creg = new Registry<ICalibrationAlg>();
    threadpool = 0;
    numthread=4;
    calibration=0;
    mutex = new QMutex(QMutex::Recursive);
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
    if(stopthread)
    {
        Logger(Error) << "CALIMERO CORE CRASH --- PLEASE RESTART";
        return false;
    };

    if(getCalibrationState()!=CALIBRATIONNOTRUNNING)
    {
        Logger(Warning) << "Calibration is already started";
        return false;
    }

    setCalibrationState(CALIBRATIONINIT);

    return true;
}

void CalibrationEnv::stopCalibration()
{
    if(stopthread)
    {
        Logger(Error) << "CALIMERO CORE CRASH --- PLEASE RESTART CALIMERO";
        return;
    }

    if(getCalibrationState()==CALIBRATIONNOTRUNNING)
    {
        Logger(Standard) << "No calibration running";
        return;
    }

    setCalibrationState(CALIBRATIONSHUTDOWN);
}

bool CalibrationEnv::setCalibration(Calibration *cal)
{
    if(getCalibrationState()!=CALIBRATIONNOTRUNNING)
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
    if(getCalibrationState()!=CALIBRATIONNOTRUNNING)
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
    assert (calibration);
    return calibration;
}

void CalibrationEnv::run()
{
    while(!stopthread)
    {
        if(getCalibrationState()==CALIBRATIONINIT)
        {
            Logger(Standard) << "Calibration started";
            runCalibration();
            setCalibrationState(CALIBRATIONNOTRUNNING);
            Logger(Standard) << "Calibration stopped";
        }

        msleep(10);
    }
}

void CalibrationEnv::runCalibration()
{
    setCalibrationState(CALIBRATIONRUNNING);

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

    ICalibrationAlg *tmpalg;

    try
    {
         tmpalg = creg->getFunction(calibration->getCalibrationAlg());
    }
    catch(const PythonException &exception)
    {
        Logger(Error) << exception.type;
        Logger(Error) << exception.traceback;
        Logger(Error) << exception.value;
    }
    catch(const CalimeroException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
    }

    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getCalibrationAlgSettings())
            tmpalg->setValueOfParameter(p.first,p.second);

    int realthreads = 0;

    if(tmpalg->containsParameter("parallel"))
    {
        realthreads = (boost::lexical_cast<double>(tmpalg->getValueOfParameter("parallel"))) ? numthread : 1;
    }
    else
        realthreads = 1;

    Logger(Standard) << "Enabled cores: " << realthreads;

    threadpool = new ModelSimThreadPool(realthreads);

    //extract parameters
    vector<CalibrationVariable*> newcalpars;
    vector<ObjectiveFunctionVariable*> newopars;

    set<string> calpars = calibration->evalCalibrationParameters();
    set<string> opars = calibration->evalObjectiveFunctionParameters();

    for(set<string>::const_iterator it = calpars.begin(); it != calpars.end(); ++it)
        newcalpars.push_back(static_cast<CalibrationVariable*>(calibration->getDomain()->getPar(*it)));

    for(set<string>::const_iterator it = opars.begin(); it != opars.end(); ++it)
        newopars.push_back(static_cast<ObjectiveFunctionVariable*>(calibration->getDomain()->getPar(*it)));

    try
    {
        if(!tmpalg->start(newcalpars,newopars,this,calibration))
            Logger(Error) << "Calibration algorithm terminates with failure";
    }
    catch(const PythonException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
        Logger(Error) << exception.type;
        Logger(Error) << exception.traceback;
        Logger(Error) << exception.value;
    }
    catch(const CalimeroException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
    }

    delete threadpool;
    delete tmpalg;
}

bool CalibrationEnv::isCalibrationRunning()
{
    return (getCalibrationState()==CALIBRATIONNOTRUNNING) ? 0 : 1;
}

bool CalibrationEnv::execIteration(vector<CalibrationVariable*> calibrationparameters)
{
    if(getCalibrationState()==CALIBRATIONSHUTDOWN)
    {
        Logger(Error) << "Shut down running calibration";
        return false;
    }

    return threadpool->pushIteration(calibrationparameters,calibration);
}

vector<string> CalibrationEnv::getAvailableObjectiveFunctions()
{
    return getObjectiveFunctionReg()->getAvailableFunctions();
}

vector<string> CalibrationEnv::getAvailableModelSimulators()
{
    return getModelSimulatorReg()->getAvailableFunctions();
}

vector<string> CalibrationEnv::getAvailableCalibrationAlgs()
{
    return getCalibrationAlgReg()->getAvailableFunctions();
}

CalibrationEnv::CALIBRATIONSTATE CalibrationEnv::getCalibrationState()
{
    lock.lockForRead();
    CALIBRATIONSTATE result = calstate;
    lock.unlock();
    return result;
}

void CalibrationEnv::setCalibrationState(CALIBRATIONSTATE state)
{
    lock.lockForWrite();
    calstate=state;
    lock.unlock();
}
