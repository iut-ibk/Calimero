/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include <CalibrationEnv.h>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <Domain.h>
#include <assert.h>
#include <Exception.h>
#include <QTime>
#include <boost/thread/thread.hpp>
#include <IResultHandler.h>

CalibrationEnv* CalibrationEnv::instance = 0;

CalibrationEnv::CalibrationEnv()
{
    stopthread=false;
    inmemory=true;
    calstate=CALIBRATIONNOTRUNNING;
    oreg = new Registry<IObjectiveFunction>();
    mreg = new  Registry<IModelSimulator>();
    creg = new Registry<ICalibrationAlg>();
    rreg = new Registry<IResultHandler>();
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
    delete rreg;
    instance=0;
}

bool CalibrationEnv::inMemory()
{
    return inmemory;
}

void CalibrationEnv::setInMemory(bool inmemory)
{
    this->inmemory=inmemory;
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
    return oreg;
}

Registry<IModelSimulator>* CalibrationEnv::getModelSimulatorReg()
{
    return mreg;
}


Registry<IResultHandler>* CalibrationEnv::getResultHandlerReg()
{
    return rreg;
}

Registry<ICalibrationAlg>* CalibrationEnv::getCalibrationAlgReg()
{
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
            try{
                QTime time;
                time.start();
                runCalibration();
                setCalibrationState(CALIBRATIONNOTRUNNING);
                int elapsed = time.elapsed();
                Logger(Standard) << "Samples/Sec: " << QString::number(calibration->getNumOfComplete()/(elapsed/(double)1000));
                Logger(Standard) << "Time elapsed: " << QString::number(elapsed) << "ms";
                Logger(Standard) << "Calibration stopped";
            }
            catch(PythonException &exception)
            {
                Logger(Error) << exception.exceptionmsg;
                Logger(Error) << exception.type;
                Logger(Error) << exception.value;
                Logger(Error) << exception.traceback;
                setCalibrationState(CALIBRATIONNOTRUNNING);
                Logger(Standard) << "Calibration stopped";
            }
            catch(CalimeroException &exception)
            {
                Logger(Error) << exception.exceptionmsg;
                setCalibrationState(CALIBRATIONNOTRUNNING);
                Logger(Standard) << "Calibration stopped";
            }
        }

        msleep(10);
    }
}

void CalibrationEnv::runAllEnabledResultHandler()
{
    map<string, string> handlers = calibration->getResultHandlers();
    std::pair<string, string>p;
    BOOST_FOREACH(p,handlers)
            if(calibration->isResultHandlerEnabled(p.first))
                runResultHandler(p.first);
}

void CalibrationEnv::runResultHandler(string name)
{
    try{
        map<string, string> handlers = calibration->getResultHandlers();
        if(handlers.find(name)==handlers.end())
            return;

        IResultHandler *function = CalibrationEnv::getInstance()->getResultHandlerReg()->getFunction(handlers[name]);
        function->setValues(calibration->getResultHandlerSettings(name));
        function->run(calibration->getIterationResults());
        delete function;
    }
    catch(PythonException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
        Logger(Error) << exception.traceback;
        Logger(Error) << exception.type;
        Logger(Error) << exception.value;
    }
    catch(CalimeroException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
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

    ICalibrationAlg *tmpalg = 0;

    try
    {
         tmpalg = creg->getFunction(calibration->getCalibrationAlg());
    }
    catch(const CalimeroException &exception)
    {
        if(!tmpalg)
            delete tmpalg;
        Logger(Error) << exception.exceptionmsg;
        return;
    }

    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getCalibrationAlgSettings())
            tmpalg->setValueOfParameter(p.first,p.second);

    if(tmpalg->containsParameter("clean results"))
    {
        if(boost::lexical_cast<int>(tmpalg->getValueOfParameter("clean results")))
            calibration->clearIterationResults();
    }
    else
        calibration->clearIterationResults();

    //threads
    numthread = QThread::idealThreadCount ();
    int realthreads = 0;
    if(tmpalg->containsParameter("parallel"))
        realthreads = (boost::lexical_cast<int>(tmpalg->getValueOfParameter("parallel")));
    else
        realthreads = 1;

    Logger(Standard) << "Enabled threads: " << realthreads;

    //disableautothreads
    bool disableautothreads = false;
    if(tmpalg->containsParameter("disableautothreads"))
        disableautothreads = (boost::lexical_cast<bool>(tmpalg->getValueOfParameter("disableautothreads")));

    Logger(Standard) << "Disable autothreads: " << disableautothreads;

    threadpool = new ModelSimThreadPool(realthreads,disableautothreads);

    int startiteration = this->getCalibration()->getNumOfComplete();

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
    catch(const CalimeroException &exception)
    {
        Logger(Error) << exception.exceptionmsg;
    }

    delete threadpool;
    delete tmpalg;
    Logger(Standard) << "Samples: " << (this->getCalibration()->getNumOfComplete()-startiteration);
}

bool CalibrationEnv::isCalibrationRunning()
{
    return (getCalibrationState()==CALIBRATIONNOTRUNNING) ? 0 : 1;
}

bool CalibrationEnv::calibrationShutDownState()
{
    return (getCalibrationState()==CALIBRATIONSHUTDOWN) ? 1 : 0;
}

bool CalibrationEnv::execIteration(vector<CalibrationVariable*> calibrationparameters)
{
    if(getCalibrationState()==CALIBRATIONSHUTDOWN)
    {
        Logger(Standard) << "Shut down running calibration";
        return false;
    }

    return threadpool->pushIteration(calibrationparameters,calibration);
}

void CalibrationEnv::barrier()
{
    Logger(Debug) << "barrier called";
    threadpool->waitForDone();
    Logger(Debug) << "barrier returns";
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

vector<string> CalibrationEnv::getAvailableResultHandlers()
{
    return getResultHandlerReg()->getAvailableFunctions();
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
