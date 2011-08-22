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
#include <Calibration.h>
#include <Logger.h>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <string>
#include <boost/foreach.hpp>
#include <Registry.h>
#include <CalibrationEnv.h>
#include <Domain.h>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <Exception.h>

using namespace boost;

Calibration::Calibration()
{
    alg = "";
    simulator = "";
    domain = new Domain();
    externalfilehandler = new ExternalParameterRegistry();
    mutex = new QMutex(QMutex::Recursive);
}

Calibration::~Calibration()
{
    clear();
    delete domain;
    delete externalfilehandler;
    delete mutex;
}

void Calibration::clear()
{
    QMutexLocker locker(mutex);
    alg="";
    simulator = "";

    delete domain;
    delete externalfilehandler;

    domain = new Domain();
    externalfilehandler = new ExternalParameterRegistry();

    clearIterationResults();

    std::pair<string, set<string>* >p;
    BOOST_FOREACH(p,groups)
            delete p.second;

    resulthandlers.clear();
    resulthandlersettings.clear();
    enabledresulthandlers.clear();

    calibrationparameters.clear();
    iterationparameters.clear();
    observedparameters.clear();
    objectivefunctionparameters.clear();
    enabledgroups.clear();
    disabledgroups.clear();
    enabledobjectivefunctionparameters.clear();
    algsettings.clear();
    modelsimulatorsettings.clear();
    groups.clear();
    Logger(Standard) << "Calimero project cleaned";
}

bool Calibration::setCalibrationAlg(string ca, map<string,string>  settings)
{
    QMutexLocker locker(mutex);
    if(!CalibrationEnv::getInstance()->getCalibrationAlgReg()->contains(ca))
    {
        Logger(Error) <<  "No calibration algorithm registered with name \"" << ca << "\"";
        return false;
    }

    ICalibrationAlg *testalg=0;
    try
    {
        testalg = CalibrationEnv::getInstance()->getCalibrationAlgReg()->getFunction(ca);

        std::pair<string,string> p;
        BOOST_FOREACH(p,settings)
                if(!testalg->containsParameter(p.first))
                {
                    delete testalg;
                    Logger(Error) <<  "Wrong values for \"" << ca << "\"";
                    return false;
                }
        alg=ca;
        algsettings=settings;
        delete testalg;
        return true;
    }
    catch(CalimeroException &exception)
    {
        if(!testalg)
            return testalg;
        Logger(Error) << exception.exceptionmsg;
    }
    return false;
}

bool Calibration::setModelSimulator(string ms, map<string,string> settings)
{
    QMutexLocker locker(mutex);
    if(!CalibrationEnv::getInstance()->getModelSimulatorReg()->contains(ms))
    {
        Logger(Error) <<  "No model simulator registered with name \"" << ms << "\"";
        return false;
    }

    IModelSimulator *testsim=0;
    try{

        testsim = CalibrationEnv::getInstance()->getModelSimulatorReg()->getFunction(ms);

        std::pair<string,string> p;
        BOOST_FOREACH(p,settings)
                if(!testsim->containsParameter(p.first))
                {
                    delete testsim;
                    Logger(Error) <<  "Wrong values for \"" << ms << "\"";
                    return false;
                }
        simulator=ms;
        modelsimulatorsettings=settings;
        return true;
    }
    catch (CalimeroException e)
    {
        if(!testsim)
            delete testsim;
        Logger(Error) << e.exceptionmsg;
    }
    return false;
}

string Calibration::getCalibrationAlg()
{
    return alg;
}

string Calibration::getModelSimulator()
{
    return simulator;
}

bool Calibration::addParameter(Variable *parameter)
{
    QMutexLocker locker(mutex);
    string parname = parameter->getName();

    if(parameter->getName()=="iteration")
    {
        Logger(Warning) << "Parameter with name \"iteration\" is a key parameter of calimero";
        return false;
    }

    if(domain->contains(parname))
    {
        Logger(Error) << parameter << " already registered in calibration domain";
        return false;
    }

    switch(parameter->getType())
    {
    case ITERATIONVARIABLE:
        iterationparameters.insert(parname);
        break;

    case OBSERVEDVARIABLE:
        observedparameters.insert(parname);
        break;

    case OBJECTIVEFUNCTIONVARIABLE:
        objectivefunctionparameters.insert(parname);
        break;

    case CALIBRATIONVARIABLE:
        if(!groups.size())
            addGroup(DEFAULTGROUP);
        calibrationparameters.insert(parname);
        groups[DEFAULTGROUP]->insert(parname);
    }

    domain->setPar(parameter);
    Logger(Standard) << parameter << " registered for calibration";
    return true;
}

bool Calibration::removeParameter(string parname)
{
    QMutexLocker locker(mutex);
    if(!domain->contains(parname))
        return false;

    if(externalfilehandler->containsParameter(parname))
        return false;

    Variable * parameter = domain->getPar(parname);

    switch(parameter->getType())
    {
    case ITERATIONVARIABLE:
        iterationparameters.erase(iterationparameters.find(parname));
        break;

    case OBSERVEDVARIABLE:
        observedparameters.erase(observedparameters.find(parname));
        break;

    case OBJECTIVEFUNCTIONVARIABLE:
        removeEnabledOParameter(parname);
        objectivefunctionparameters.erase(objectivefunctionparameters.find(parname));
        break;

    case CALIBRATIONVARIABLE:
        {
            std::pair<string, set<string>* >p;
            BOOST_FOREACH(p,groups)
                    if(p.second->find(parname)!=p.second->end())
                        p.second->erase(p.second->find(parname));

        calibrationparameters.erase(calibrationparameters.find(parname));
        }
    }

    domain->removePar(parname);

    Logger(Debug) << "[" << parname << "] removed from registered calibration";
    return true;
}

bool Calibration::addGroup(std::string name)
{
    QMutexLocker locker(mutex);
    if(groups.find(name)!=groups.end())
        return false;

    groups[name] = new set<string>();

    if(name==DEFAULTGROUP)
        enabledgroups[name]=true;
    else
        enabledgroups[name]=false;

    disabledgroups[name]=false;

    return true;
}

bool Calibration::removeGroup(std::string name)
{
    QMutexLocker locker(mutex);
    if(groups.find(name)==groups.end())
        return false;

    if(name==DEFAULTGROUP)
        return false;

    delete groups[name];
    removeEnabledGroup(name);
    removeDisabledGroup(name);
    enabledgroups.erase(name);
    disabledgroups.erase(name);
    groups.erase (groups.find(name));
    return true;
}

bool Calibration::containsParameter(string var)
{
    return domain->contains(var);
}

bool Calibration::containsGroup(std::string groupname)
{
    return groups.find(groupname)!=groups.end();
}

bool Calibration::addParameterToGroup(string var, std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsParameter(var))
        return false;

    if(!containsGroup(groupname))
        return false;

    Variable *tmpvar = domain->getPar(var);
    if(tmpvar->getType()!=CALIBRATIONVARIABLE)
        return false;

    groups[groupname]->insert(var);
    return true;
}

bool Calibration::removeParameterFromGroup(string var, std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsGroup(groupname) || !containsParameter(var))
        return false;

    Variable *tmpvar = domain->getPar(var);
    if(tmpvar->getType()!=CALIBRATIONVARIABLE)
        return false;

    if(groupname==DEFAULTGROUP)
        return false;

    groups[groupname]->erase(groups[groupname]->find(var));
    return true;
}

bool Calibration::addEnabledGroup(std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsGroup(groupname))
        return false;

    enabledgroups[groupname]=true;
    return true;
}

bool Calibration::removeEnabledGroup(std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsGroup(groupname))
        return false;

    enabledgroups[groupname]=false;

    return true;
}

bool Calibration::addDisabledGroup(std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsGroup(groupname))
        return false;

    disabledgroups[groupname]=true;
    return true;
}

bool Calibration::removeDisabledGroup(std::string groupname)
{
    QMutexLocker locker(mutex);
    if(!containsGroup(groupname))
        return false;

    disabledgroups[groupname]=false;

    return true;
}

bool Calibration::addEnabledOParameter(string parameter)
{
    QMutexLocker locker(mutex);
    if(!containsParameter(parameter))
        return false;

    Variable *tmpvar = domain->getPar(parameter);
    if(tmpvar->getType()!=OBJECTIVEFUNCTIONVARIABLE)
        return false;


    enabledobjectivefunctionparameters.insert(parameter);
    return true;
}

bool Calibration::removeEnabledOParameter(string parameter)
{
    QMutexLocker locker(mutex);
    if(!containsParameter(parameter))
        return false;

    Variable *tmpvar = domain->getPar(parameter);
    if(tmpvar->getType()!=OBJECTIVEFUNCTIONVARIABLE)
        return false;

    if(enabledobjectivefunctionparameters.find(parameter)==enabledobjectivefunctionparameters.end())
        return false;

    enabledobjectivefunctionparameters.erase(enabledobjectivefunctionparameters.find(parameter));
    return true;
}

set<string> Calibration::evalCalibrationParameters()
{
    set<string> tmp;

    //add all enabled parameters
    std::pair<string, bool> p;

    BOOST_FOREACH(p, enabledgroups)
            if(p.second)
            {
                for (set<string>::const_iterator it = groups[p.first]->begin(); it != groups[p.first]->end(); it++)
                    tmp.insert(*it);
             }

    //remove all disabled parameters
    BOOST_FOREACH(p, disabledgroups)
            if(p.second)
            {
                for (set<string>::const_iterator it = groups[p.first]->begin(); it != groups[p.first]->end(); it++)
                    tmp.erase(tmp.find(*it));
            }

    return tmp;
}

int Calibration::getNumOfComplete()
{
    QMutexLocker locker(mutex);
    int result=0;
    std::pair<int,IterationResult * > p;
    BOOST_FOREACH(p, iterationresults)
            result += (p.second->isComplete()) ? 1 : 0 ;

    return result;
}

vector<IterationResult *  > Calibration::getIterationResults()
{
    QMutexLocker locker(mutex);
    vector<IterationResult *  > result;
    std::pair<int,IterationResult *  >p;
    BOOST_FOREACH(p,iterationresults)
            if(p.second->isComplete())
                result.push_back(p.second);
    return result;
}

void Calibration::clearIterationResults()
{
    QMutexLocker locker(mutex);
    for(uint index=0; index<iterationresults.size(); index++)
        delete iterationresults[index];

    iterationresults.clear();
}

map<string,string> Calibration::getCalibrationAlgSettings()
{
    return algsettings;
}

map<string,string> Calibration::getModelSimulatorSettings()
{
    return modelsimulatorsettings;
}

IterationResult *   Calibration::newIterationResult()
{
    QMutexLocker locker(mutex);
    iterationresults.insert(pair<int,IterationResult * >(iterationresults.size(), new IterationResult(iterationresults.size())));
    return iterationresults[iterationresults.size()-1];
}

vector<string> Calibration::getAllCalibrationParameters()
{
    vector<string> result;
    copy ( calibrationparameters.begin(), calibrationparameters.end(), result.begin() );
    return result;
}

vector<string> Calibration::getAllObservedParameters()
{
    vector<string> result;
    copy ( observedparameters.begin(), observedparameters.end(), result.begin() );
    return result;
}

vector<string> Calibration::getAllIterationParameters()
{
    vector<string> result;
    copy ( iterationparameters.begin(), iterationparameters.end(), result.begin() );
    return result;
}

vector<string> Calibration::getAllObjectiveFunctionParameters()
{
    vector<string> result;
    copy ( objectivefunctionparameters.begin(), objectivefunctionparameters.end(), result.begin() );
    return result;
}

set<string> Calibration::evalObjectiveFunctionParameters()
{
    return enabledobjectivefunctionparameters;
}

Domain* Calibration::getDomain()
{
    return domain;
}

ExternalParameterRegistry* Calibration::getExternalParameterRegistry()
{
    return externalfilehandler;
}

bool Calibration::containsGroupMember(string varname, string groupname)
{
    if(!containsGroup(groupname))
        return false;

    return groups[groupname]->find(varname)!=groups[groupname]->end();
}

vector<string> Calibration::getAllGroups()
{
    if(!groups.size())
    {
        addGroup(DEFAULTGROUP);
    }

    vector<string> result;
    std::pair<string, set<string>* > p;
    BOOST_FOREACH(p,groups)
            result.push_back(p.first);

    return result;
}

map<string,bool> Calibration::getDisabledGroups()
{
    return disabledgroups;
}

map<string,bool> Calibration::getEnabledGroups()
{
    return enabledgroups;
}

set<string> Calibration::getGroupMembers(const string &name)
{
    if(!containsGroup(name))
        return set<string>();

    return *groups[name];
}

bool Calibration::isEnabledGroup(const string &name)
{
    if(!containsGroup(name))
        return false;

    return enabledgroups[name];
}

bool Calibration::isDisabledGroup(const string &name)
{
    if(!containsGroup(name))
        return false;

    return disabledgroups[name];
}

bool Calibration::setIterationResults(map<int,IterationResult *  > iterationresults)
{
    clearIterationResults();
    this->iterationresults=iterationresults;
    return true;
}

map<string, string> Calibration::getResultHandlers()
{
    return resulthandlers;
}

map<string, string> Calibration::getResultHandlerSettings(string name)
{
    return resulthandlersettings[name];
}

bool Calibration::isResultHandlerEnabled(string name)
{
    return enabledresulthandlers[name];
}

bool Calibration::addResultHandler(string name, string functionname, map<string,string> settings, bool enabled)
{
    if(!CalibrationEnv::getInstance()->getResultHandlerReg()->contains(functionname))
    {
        Logger(Error) <<  "No result handler registered with name \"" << functionname << "\"";
        return false;
    }
    resulthandlers[name]=functionname;
    resulthandlersettings[name]=settings;
    enabledresulthandlers[name]=enabled;
    return true;
}

bool Calibration::removeResultHandler(string name)
{
    if(resulthandlers.find(name)==resulthandlers.end())
        return false;

    resulthandlers.erase(resulthandlers.find(name));
    resulthandlersettings.erase(resulthandlersettings.find(name));
    enabledresulthandlers.erase(enabledresulthandlers.find(name));
    return true;
}
