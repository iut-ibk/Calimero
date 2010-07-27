#include <Domain.h>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <CalibrationVariable.h>
#include <assert.h>

Domain::Domain(){
}

Domain::~Domain()
{
    std::pair<string,Variable*> p;

    map<string, Variable*> mem = members;
    BOOST_FOREACH(p,mem)
            removePar(p.first);
}

void Domain::setPar(Variable* var)
{
    if(contains(var->getName()))
        delete members[var->getName()];

    Variable *newvar;

    switch(var->getType())
    {
    case OBJECTIVEFUNCTIONVARIABLE:
            newvar = new ObjectiveFunctionVariable(*(static_cast<const ObjectiveFunctionVariable*>(var)));
            break;
    case CALIBRATIONVARIABLE:
            newvar = new CalibrationVariable(*(static_cast<const CalibrationVariable*>(var)));
            break;
    default:
            newvar = new Variable(*var);
            break;
    }

    members[var->getName()]=newvar;
    newvar->setDomain(this);
}

Variable* Domain::getPar(const string &name)
{
    if(contains(name))
            return members[name];

    Logger(Warning) << "Domain return null pointer";
    return 0;
}

bool Domain::removePar(const string &name)
{
    if(!contains(name))
        return false;

    Variable *var  = getPar(name);
    delete var;

    members.erase(members.find(name));
    return true;
}

bool Domain::contains(string var)
{
    return (members.find(var)!=members.end());
}

Domain::Domain(const Domain &olddomain)
{
    std::pair<string, Variable*>p;
    BOOST_FOREACH(p, olddomain.members)
        setPar(p.second);
}

vector<Variable*> Domain::getAllPars(const VARTYPE &type)
{
    vector<Variable*> result;
    std::pair<string, Variable*> p;

    BOOST_FOREACH(p,members)
            if(p.second->getType()==type)
                result.push_back(p.second);

    return result;
}
