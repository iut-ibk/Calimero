#include <Calibration.h>
#include <Logger.h>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <string>
#include <boost/foreach.hpp>
#include <Registry.h>

Calibration* Calibration::calibration = 0;

Calibration::Calibration()
{
    alg = "";
    calstate=CALIBRATIONNOTRUNNING;
}

Calibration::~Calibration()
{
    clear();
}

void Calibration::clear()
{
    alg="";

    set<Variable*>tmpiterationp=iterationparameters;
    BOOST_FOREACH(Variable* variable, tmpiterationp)
            removeParameter(variable);

    set<Variable*>tmpobservedp=observedparameters;
    BOOST_FOREACH(Variable* variable, tmpobservedp)
            removeParameter(variable);

    set<ObjectiveFunctionVariable*>tmpobjectivefunctionp=objectivefunctionparameters;
    BOOST_FOREACH(ObjectiveFunctionVariable* variable, tmpobjectivefunctionp)
            removeParameter(variable);

    set<CalibrationVariable*>tmpcalibrationp=calibrationparameters;
    BOOST_FOREACH(CalibrationVariable* variable, tmpcalibrationp)
            removeParameter(variable);

    map<string, set<CalibrationVariable*>*>tmpgroups=groups;
    for( map<string, set<CalibrationVariable*>*>::iterator ii=tmpgroups.begin(); ii!=tmpgroups.end(); ++ii)
           delete (*ii).second;

    clearIterationResults();
}

bool Calibration::startCalibration()
{
    if(alg=="")
    {
        Logger(Error) << "No calibration algorithm set";
        return false;
    }

    if(calstate!=CALIBRATIONNOTRUNNING)
    {
        Logger(Error) << "Calibration is already started";
        return false;
    }

    //TODO implement
    Logger(Debug) << "startCalibration not correct implemented yet";

    ICalibrationAlg *tmpalg = creg->getFunction(alg);
    tmpalg->start();
    delete tmpalg;



    return true;
}

void Calibration::stopCalibration()
{
    if(calstate==CALIBRATIONNOTRUNNING)
    {
        Logger(Standard) << "No calibration running";
        return;
    }

    calstate=CALIBRATIONSHUTDOWN;
}

Calibration* Calibration::getInstance()
{
    if(!calibration)
        calibration = new Calibration();

    return calibration;
}

bool Calibration::setCalibrationAlg(string ca)
{   
    if(!creg->contains(ca))
    {
        Logger(Error) <<  "No calibration algorithm registered with name \"" << ca << "\"";
        return false;
    }

    alg=ca;
    return true;
}

string Calibration::getCalibrationAlg()
{
    return alg;
}

bool Calibration::addParameter(Variable *parameter)
{
    if(parameter==NULL)
        return false;

    if(containsParameter(parameter))
        return false;

    switch(parameter->getType())
    {
    case Variable::ITERATIONVARIABLE:
        iterationparameters.insert(parameter);
        break;

    case Variable::OBSERVEDVARIABLE:
        observedparameters.insert(parameter);
        break;

    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        objectivefunctionparameters.insert(static_cast<ObjectiveFunctionVariable*>(parameter));
        break;

    case Variable::CALIBRATIONVARIABLE:
        if(!groups.size())
            groups["default"] = new set<CalibrationVariable*>();
        calibrationparameters.insert(static_cast<CalibrationVariable*>(parameter));
        groups["default"]->insert(static_cast<CalibrationVariable*>(parameter));
        return false;
    }

    Logger(Standard) << parameter << " registered for calibration";
    return true;
}

bool Calibration::removeParameter(Variable *parameter)
{
    if(parameter==NULL)
        return false;

    if(!containsParameter(parameter))
        return false;

    switch(parameter->getType())
    {
    case Variable::ITERATIONVARIABLE:
        iterationparameters.erase(iterationparameters.find(parameter));
        break;

    case Variable::OBSERVEDVARIABLE:
        observedparameters.erase(observedparameters.find(parameter));
        break;

    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        removeEnabledOParameter(static_cast<ObjectiveFunctionVariable*>(parameter));
        objectivefunctionparameters.erase(objectivefunctionparameters.find(static_cast<ObjectiveFunctionVariable*>(parameter)));
        break;

    case Variable::CALIBRATIONVARIABLE:
        for( map<string, set<CalibrationVariable*>*>::iterator ii=groups.begin(); ii!=groups.end(); ++ii)
            if((*ii).second->find(static_cast<CalibrationVariable*>(parameter))!=(*ii).second->end())
                (*ii).second->erase((*ii).second->find(static_cast<CalibrationVariable*>(parameter)));

        calibrationparameters.erase(calibrationparameters.find(static_cast<CalibrationVariable*>(parameter)));
    }

    Logger(Standard) << parameter << " removed from registered calibration";
    return true;
}

bool Calibration::addGroup(std::string name)
{
    if(groups.find(name)==groups.end())
        return false;

    groups[name] = new set<CalibrationVariable*>();
    return true;
}

bool Calibration::removeGroup(std::string name)
{
    if(groups.find(name)!=groups.end())
        return false;

    delete groups[name];
    groups.erase (groups.find(name));
    return true;
}

bool Calibration::containsParameter(Variable* var)
{
    switch(var->getType())
    {
    case Variable::ITERATIONVARIABLE:
        return iterationparameters.find(var)!=iterationparameters.end();

    case Variable::OBSERVEDVARIABLE:
        return observedparameters.find(var)!=observedparameters.end();

    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        return objectivefunctionparameters.find(static_cast<ObjectiveFunctionVariable*>(var))!=objectivefunctionparameters.end();

    case Variable::CALIBRATIONVARIABLE:
        return calibrationparameters.find(static_cast<CalibrationVariable*>(var))!=calibrationparameters.end();
    }

    return false;
}

bool Calibration::containsGroup(std::string groupname)
{
    return groups.find(groupname)!=groups.end();
}

bool Calibration::addParameterToGroup(CalibrationVariable* var, std::string groupname)
{
    if(!var)
        return false;

    if(!containsGroup(groupname))
        return false;

    groups[groupname]->insert(var);
    return true;
}

bool Calibration::removeParameterFromGroup(CalibrationVariable* var, std::string groupname)
{
    if(!containsGroup(groupname) || !containsParameter(var))
        return false;

    groups[groupname]->erase(groups[groupname]->find(var));
    return true;
}

bool Calibration::addEnabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    enabledgroups.insert(groups[groupname]);
    return true;
}

bool Calibration::removeEnabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    if(enabledgroups.find(groups[groupname])==enabledgroups.end())
        return false;

    enabledgroups.erase(enabledgroups.find(groups[groupname]));
    return true;
}

bool Calibration::addDisabledGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    disabledgroups.insert(groups[groupname]);
    return true;
}

bool Calibration::removeDisableGroup(std::string groupname)
{
    if(!containsGroup(groupname))
        return false;

    if(disabledgroups.find(groups[groupname])==disabledgroups.end())
        return false;

    disabledgroups.erase(disabledgroups.find(groups[groupname]));
    return true;
}

bool Calibration::addEnabledOParameter(ObjectiveFunctionVariable* parameter)
{
    if(!containsParameter(parameter))
        return false;

    enabledobjectivefunctionparameters.insert(parameter);
    return true;
}

bool Calibration::removeEnabledOParameter(ObjectiveFunctionVariable* parameter)
{
    if(!containsParameter(parameter))
        return false;

    if(enabledobjectivefunctionparameters.find(parameter)==enabledobjectivefunctionparameters.end())
        return false;

    enabledobjectivefunctionparameters.erase(enabledobjectivefunctionparameters.find(parameter));
    return true;
}

vector<CalibrationVariable*> Calibration::evalCalibrationParameters()
{
    set<CalibrationVariable*> tmp;

    //add all enabled parameters
    BOOST_FOREACH(set<CalibrationVariable* > *tmpset, enabledgroups)
             BOOST_FOREACH(CalibrationVariable *tmpvar, *tmpset)
                tmp.insert(tmpvar);

    //remove all disabled parameters
    BOOST_FOREACH(set<CalibrationVariable* > *tmpset, disabledgroups)
             BOOST_FOREACH(CalibrationVariable *tmpvar, *tmpset)
                if(tmp.find(tmpvar)!=tmp.end())
                    tmp.erase(tmp.find(tmpvar));

    //extract result
    vector<CalibrationVariable*> result;

    BOOST_FOREACH(CalibrationVariable* var, tmp)
            result.assign(1,var);

    return result;
}

vector<ObjectiveFunctionVariable*> Calibration::evalObjectiveParameters()
{
    vector<ObjectiveFunctionVariable*> result;

    BOOST_FOREACH(ObjectiveFunctionVariable* var, enabledobjectivefunctionparameters)
            result.assign(1,var);

    return result;
}

int Calibration::getNumOfComplete()
{
    int result=0;
    BOOST_FOREACH(IterationResult* iteration, iterationresults)
            result += (iteration->isComplete()) ? 1 : 0 ;

    return result;
}

vector<IterationResult*> Calibration::getIterationResult()
{
    vector<IterationResult*> result;

    BOOST_FOREACH(IterationResult* iteration, iterationresults)
            if(iteration->isComplete())
                result.assign(1,iteration);

    return result;
}

void Calibration::clearIterationResults()
{
    BOOST_FOREACH(IterationResult* result, iterationresults)
            delete result;

    iterationresults.clear();
}

bool Calibration::exec(vector<CalibrationVariable*> calibrationparameters,
          vector<Variable*> observedparameters,
          vector<Variable*> iterationparameters,
          vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{





    Logger(Error) << "Calibration::exec not implemented yet";
    return false;
}

Variable* Calibration::cloneParameter(Variable* old)
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

Registry<IObjectiveFunction>* Calibration::getObjectiveFunctionReg()
{
    if(!oreg)
        oreg = new Registry<IObjectiveFunction>();
    return oreg;
}

Registry<IModelSimulator>* Calibration::getModelSimulatorReg()
{
    if(!mreg)
        mreg = new Registry<IModelSimulator>();
    return mreg;
}

Registry<ICalibrationAlg>* Calibration::getCalibrationAlgReg()
{
    if(!creg)
        creg = new Registry<ICalibrationAlg>();
    return creg;
}
