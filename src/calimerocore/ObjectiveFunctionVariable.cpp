#include <ObjectiveFunctionVariable.h>
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <Logger.h>
#include <Registry.h>

using namespace std;

ObjectiveFunctionVariable::ObjectiveFunctionVariable(string Name) : Variable(Name, vector<double>(),OBJECTIVEFUNCTIONVARIABLE)
{
    needupdate = false;
    function="";
}

ObjectiveFunctionVariable::~ObjectiveFunctionVariable()
{
    BOOST_FOREACH(Variable* variable, iterationparameters)
            removeParameter(variable);

    BOOST_FOREACH(Variable* variable, observedparameters)
            removeParameter(variable);

    BOOST_FOREACH(ObjectiveFunctionVariable* variable, objectivefunctionparameters)
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

    BOOST_FOREACH(ObjectiveFunctionVariable* variable, objectivefunctionparameters)
        if(variable==var || !variable->parameterCycleCheck(var))
            return false;

    return true;
}

bool ObjectiveFunctionVariable::addParameter(Variable* var)
{
    if(var==NULL)
        return false;

    if(containsParameter(var))
        return false;

    switch(var->getType())
    {
    case Variable::ITERATIONVARIABLE:
        iterationparameters.insert(var);;
        break;

    case Variable::OBSERVEDVARIABLE:
        observedparameters.insert(var);
        break;

    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        if(!parameterCycleCheck(static_cast<ObjectiveFunctionVariable*>(var)))
            objectivefunctionparameters.insert(static_cast<ObjectiveFunctionVariable*>(var));
        break;

    case Variable::CALIBRATIONVARIABLE:
        return false;
    }

    var->addSuccessor(this);
    fireUpdate();

    return true;
}

bool ObjectiveFunctionVariable::removeParameter(Variable* var)
{
    if(var==NULL)
        return false;

    if(!containsParameter(var))
            return false;

    switch(var->getType())
    {
    case Variable::ITERATIONVARIABLE:
        iterationparameters.erase(iterationparameters.find(var));
        break;

    case Variable::OBSERVEDVARIABLE:
        observedparameters.erase(observedparameters.find(var));
        break;

    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        objectivefunctionparameters.erase(objectivefunctionparameters.find(static_cast<ObjectiveFunctionVariable*>(var)));
        break;

    case Variable::CALIBRATIONVARIABLE:
        return false;
    }

    var->removeSuccessor(this);
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

    IObjectiveFunction *tmpfunction = Registry<IObjectiveFunction>::getInstance()->getFunction(function);

    vector<Variable*> iterationvector;
    vector<Variable*> observedvector;
    vector<ObjectiveFunctionVariable*> objectivevector;

    BOOST_FOREACH(Variable *var, iterationparameters)
        iterationvector.assign(1,var);

    BOOST_FOREACH(Variable *var, observedparameters)
        observedvector.assign(1,var);

    BOOST_FOREACH(ObjectiveFunctionVariable *var, objectivefunctionparameters)
        objectivevector.assign(1,var);

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

bool ObjectiveFunctionVariable::setObjectiveFunction(std::string ofunction)
{
    if(!Registry<IObjectiveFunction>::getInstance()->contains(ofunction))
    {
        Logger(Error) << this << "No objective function registered with name \"" << ofunction <<"\"";
        return false;
    }

    function=ofunction;

    return true;
}

std::string ObjectiveFunctionVariable::getObjectiveFunction()
{
    return function;
}

set<Variable*>* ObjectiveFunctionVariable::getIterationParameters()
{
    return &iterationparameters;
}

set<Variable*>* ObjectiveFunctionVariable::getObservedParameters()
{
    return &observedparameters;
}

set<ObjectiveFunctionVariable*>* ObjectiveFunctionVariable::getObjectiveFunctionParameters()
{
    return &objectivefunctionparameters;
}

bool ObjectiveFunctionVariable::containsParameter(Variable* var)
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
        return false;
    }

    return false;
}
