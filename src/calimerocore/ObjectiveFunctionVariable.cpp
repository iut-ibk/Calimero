#include <ObjectiveFunctionVariable.h>
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <Logger.h>
#include <Registry.h>
#include <Calibration.h>
#include <CalibrationEnv.h>

using namespace std;

ObjectiveFunctionVariable::ObjectiveFunctionVariable(string Name) : Variable(Name, vector<double>(),OBJECTIVEFUNCTIONVARIABLE)
{
    needupdate = false;
    function="";
}

ObjectiveFunctionVariable::~ObjectiveFunctionVariable()
{
    BOOST_FOREACH(string variable, iterationparameters)
            removeParameter(variable);

    BOOST_FOREACH(string variable, observedparameters)
            removeParameter(variable);

    BOOST_FOREACH(string variable, objectivefunctionparameters)
            removeParameter(variable);
}

void ObjectiveFunctionVariable::fireUpdate()
{
    needupdate=true;

    Variable::fireUpdate();
}

bool ObjectiveFunctionVariable::parameterCycleCheck(ObjectiveFunctionVariable* var)
{
    if(this==var)
            return false;

    BOOST_FOREACH(string varname, objectivefunctionparameters)
    {
        ObjectiveFunctionVariable* variable = static_cast<ObjectiveFunctionVariable*>(domain->getPar(varname));
        if(variable==var || !variable->parameterCycleCheck(var))
            return false;
    }

    return true;
}

bool ObjectiveFunctionVariable::addParameter(const string &var)
{
    if(containsParameter(var))
        return false;

    if(!domain->contains(var))
            return false;

    Variable* varinstance = domain->getPar(var);

    switch(varinstance->getType())
    {
    case ITERATIONVARIABLE:
        iterationparameters.insert(var);
        break;

    case OBSERVEDVARIABLE:
        observedparameters.insert(var);
        break;

    case OBJECTIVEFUNCTIONVARIABLE:
        if(!parameterCycleCheck(static_cast<ObjectiveFunctionVariable*>(varinstance)))
            objectivefunctionparameters.insert(var);
        break;

    case CALIBRATIONVARIABLE:
        return false;
    }

    varinstance->addSuccessor(name);
    fireUpdate();

    return true;
}

bool ObjectiveFunctionVariable::removeParameter(const string &var)
{
    if(!containsParameter(var))
            return false;

    Variable* varinstance = domain->getPar(var);

    switch(varinstance->getType())
    {
    case ITERATIONVARIABLE:
        iterationparameters.erase(iterationparameters.find(var));
        break;

    case OBSERVEDVARIABLE:
        observedparameters.erase(observedparameters.find(var));
        break;

    case OBJECTIVEFUNCTIONVARIABLE:
        objectivefunctionparameters.erase(objectivefunctionparameters.find(var));
        break;

    case CALIBRATIONVARIABLE:
        return false;
    }

    varinstance->removeSuccessor(name);
    fireUpdate();

    return true;
}

bool ObjectiveFunctionVariable::calc()
{

    needupdate=false;

    if(function=="")
    {
        Logger(Warning) << this << " no objective function set";
        return false;
    }

    IObjectiveFunction *tmpfunction = CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getFunction(function);

    std::pair<string,string> p;

    BOOST_FOREACH(p, functionsettings)
            tmpfunction->setValueOfParameter(p.first,p.second);

    vector<Variable*> iterationvector;
    vector<Variable*> observedvector;
    vector<ObjectiveFunctionVariable*> objectivevector;

    BOOST_FOREACH(string var, iterationparameters)
        iterationvector.push_back(domain->getPar(var));

    BOOST_FOREACH(string var, observedparameters)
        observedvector.push_back(domain->getPar(var));

    BOOST_FOREACH(string var, objectivefunctionparameters)
        objectivevector.push_back(static_cast<ObjectiveFunctionVariable*>(domain->getPar(var)));

    values=tmpfunction->eval(&iterationvector,&observedvector,&objectivevector);
    delete tmpfunction;
    return true;
}

vector<double>  ObjectiveFunctionVariable::getValues()
{
    if(needupdate)
       calc();
    return values;
}

bool ObjectiveFunctionVariable::setValues(vector<double> value)
{
    return false;
}

bool ObjectiveFunctionVariable::setObjectiveFunction(std::string ofunction, map<string,string> settings)
{
    if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->contains(ofunction))
    {
        Logger(Error) << this << "No objective function registered with name: " << ofunction << "-";
        return false;
    }

    IObjectiveFunction *testofunc = CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getFunction(ofunction);

    std::pair<string,string> p;
    BOOST_FOREACH(p,settings)
            if(!testofunc->containsParameter(p.first))
            {
                delete testofunc;
                Logger(Error) <<  "Wrong values for \"" << ofunction << "\"";
                return false;
            }

    functionsettings=settings;
    function=ofunction;

    return true;
}

map<string,string> ObjectiveFunctionVariable::getObjectiveFunctionSettings()
{
    return functionsettings;
}

std::string ObjectiveFunctionVariable::getObjectiveFunction()
{
    return function;
}

set<string> ObjectiveFunctionVariable::getIterationParameters()
{
    return iterationparameters;
}

set<string> ObjectiveFunctionVariable::getObservedParameters()
{
    return observedparameters;
}

set<string> ObjectiveFunctionVariable::getObjectiveFunctionParameters()
{
    return objectivefunctionparameters;
}

bool ObjectiveFunctionVariable::containsParameter(const string &varname)
{
    if(!domain->contains(varname))
        return false;

    Variable* var = domain->getPar(varname);

    switch(var->getType())
    {
    case ITERATIONVARIABLE:
        return iterationparameters.find(varname)!=iterationparameters.end();

    case OBSERVEDVARIABLE:
        return observedparameters.find(varname)!=observedparameters.end();

    case OBJECTIVEFUNCTIONVARIABLE:
        return objectivefunctionparameters.find(varname)!=objectivefunctionparameters.end();

    case CALIBRATIONVARIABLE:
        return false;
    }

    return false;
}
