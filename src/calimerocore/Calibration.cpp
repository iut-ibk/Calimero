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

Calibration::Calibration()
{
    alg = "";
    domain = new Domain();
    externalfilehandler = new ExternalParameterRegistry();
}

Calibration::~Calibration()
{
    clear();
    delete domain;
    delete externalfilehandler;
}

void Calibration::clear()
{
    alg="";

    delete domain;
    delete externalfilehandler;
    domain = new Domain();
    externalfilehandler = new ExternalParameterRegistry();

    clearIterationResults();

    std::pair<string, set<string>* >p;
    BOOST_FOREACH(p,groups)
            delete p.second;

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
}

bool Calibration::setCalibrationAlg(string ca, map<string,string>  settings)
{   
    if(!CalibrationEnv::getInstance()->getCalibrationAlgReg()->contains(ca))
    {
        Logger(Error) <<  "No calibration algorithm registered with name \"" << ca << "\"";
        return false;
    }

    ICalibrationAlg *testalg = CalibrationEnv::getInstance()->getCalibrationAlgReg()->getFunction(ca);

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
    return true;
}

bool Calibration::setModelSimulator(string ms, map<string,string> settings)
{
    if(!CalibrationEnv::getInstance()->getModelSimulatorReg()->contains(ms))
    {
        Logger(Error) <<  "No model simulator registered with name \"" << ms << "\"";
        return false;
    }

    IModelSimulator *testsim = CalibrationEnv::getInstance()->getModelSimulatorReg()->getFunction(ms);

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
    string parname = parameter->getName();

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
            groups[DEFAULTGROUP] = new set<string>();
        calibrationparameters.insert(parname);
        groups[DEFAULTGROUP]->insert(parname);
    }

    domain->setPar(parameter);
    Logger(Debug) << parameter << " registered for calibration";
    return true;
}

bool Calibration::removeParameter(string parname)
{
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
    if(groups.find(name)!=groups.end())
        return false;

    groups[name] = new set<string>();
    enabledgroups[name]=false;
    disabledgroups[name]=false;
    return true;
}

bool Calibration::removeGroup(std::string name)
{
    if(groups.find(name)==groups.end())
        return false;

    if(name==DEFAULTGROUP)
        return false;

    delete groups[name];
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
    if(!containsGroup(groupname))
        return false;

    enabledgroups[groupname]=true;
    return true;
}

bool Calibration::removeEnabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    enabledgroups[groupname]=false;

    return true;
}

bool Calibration::addDisabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    disabledgroups[groupname]=true;
    return true;
}

bool Calibration::removeDisabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    disabledgroups[groupname]=false;

    return true;
}

bool Calibration::addEnabledOParameter(string parameter)
{
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
                for (set<string>::const_iterator it = groups[p.first]->begin(); it != groups[p.first]->end(); ++it)
                    tmp.insert(*it);
             }

    //remove all disabled parameters
    BOOST_FOREACH(p, disabledgroups)
            if(!p.second)
            {
                for (set<string>::const_iterator it = groups[p.first]->begin(); it != groups[p.first]->end(); ++it)
                    tmp.erase(tmp.find(*it));
            }

    return tmp;
}

int Calibration::getNumOfComplete()
{
    int result=0;
    std::pair<int,IterationResult*>p;
    BOOST_FOREACH(p, iterationresults)
            result += (p.second->isComplete()) ? 1 : 0 ;

    return result;
}

map<int,IterationResult*> Calibration::getIterationResults()
{
    return iterationresults;
}

void Calibration::clearIterationResults()
{
    std::pair<int,IterationResult*>p;
    BOOST_FOREACH(p, iterationresults)
            delete p.second;

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

IterationResult* Calibration::newIterationResult()
 {
     IterationResult* result = new IterationResult(iterationresults.size());
     iterationresults[result->getIterationNumber()]=result;
     return result;
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
        groups[DEFAULTGROUP] = new set<string>();

    vector<string> result;
    std::pair<string, set<string>* > p;
    BOOST_FOREACH(p,groups)
            result.push_back(p.first);

    return result;
}
