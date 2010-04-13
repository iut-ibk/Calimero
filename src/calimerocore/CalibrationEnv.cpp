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
        tmp->setObjectiveFunction(dynamic_cast<ObjectiveFunctionVariable*>(old)->getObjectiveFunction(),
                                  dynamic_cast<ObjectiveFunctionVariable*>(old)->getObjectiveFunctionSettings());
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

bool CalibrationEnv::exec(vector<CalibrationVariable*> calibrationparameters,
          vector<Variable*> observedparameters,
          vector<Variable*> iterationparameters,
          vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{
    if(calstate!=CALIBRATIONSHUTDOWN)
    {
        Logger(Error) << "Shut down running calibration";
        return false;
    }

    //create instance of model simulator
    IModelSimulator *tmpsim = mreg->getFunction(calibration->getModelSimulator());

    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getModelSimulatorSettings())
            tmpsim->setValueOfParameter(p.first,p.second);


    Logger(Error) << "Calibration::exec not implemented";

    //clone all parameters
    vector<CalibrationVariable*> newcalibrationparameters;
    vector<Variable*> newobservedparameters;
    vector<Variable*> newiterationparameters;
    vector<ObjectiveFunctionVariable*> newofunctions;

    BOOST_FOREACH(void* p, calibrationparameters)
            newcalibrationparameters.assign(1,(CalibrationVariable*)cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, observedparameters)
            newobservedparameters.assign(1,cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, iterationparameters)
            newiterationparameters.assign(1,cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, objectivefunctionparameters)
            newofunctions.assign(1,(ObjectiveFunctionVariable*)cloneParameter((Variable*)p));


    //link all parameters
    for(uint index = 0; index < objectivefunctionparameters.size(); index++)
    {
        //link ObjectiveFunctionVariable
        BOOST_FOREACH(ObjectiveFunctionVariable* oldofun, *(objectivefunctionparameters[index]->getObjectiveFunctionParameters()))
            BOOST_FOREACH(ObjectiveFunctionVariable* newofun, newofunctions)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }

        //link ObservedParameters
        BOOST_FOREACH(Variable* oldofun, *(objectivefunctionparameters[index]->getObservedParameters()))
            BOOST_FOREACH(Variable* newofun, observedparameters)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }

        //link IterationParameters
        BOOST_FOREACH(Variable* oldofun, *(objectivefunctionparameters[index]->getIterationParameters()))
            BOOST_FOREACH(Variable* newofun, iterationparameters)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }
    }

    threadpool->pushIteration(newcalibrationparameters,
                              newobservedparameters,
                              newiterationparameters,
                              newofunctions,
                              calibration->newIterationResult(),
                              tmpsim);
    return true;
}
