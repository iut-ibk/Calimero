#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <Domain.h>
#include <string>
#include <vector>
#include <list>
#include <boost/foreach.hpp>
#include <assert.h>

using namespace std;

Variable::Variable(string Name, vector<double> value, VARTYPE TYPE)
{
    name=Name;
    type=TYPE;
    values = value;
}

Variable::Variable(const Variable &oldvar)
{
    name=oldvar.name;
    type=oldvar.type;
    values=oldvar.values;
}

Variable::~Variable()
{
    BOOST_FOREACH( string var ,successors )
            static_cast<ObjectiveFunctionVariable*>(domain->getPar(var))->removeParameter(var);
}

vector<double> Variable::getValues()
{
    return vector<double>(values);
}

bool Variable::setValues(vector<double> value)
{
    values.clear();
    values=value;
    fireUpdate();
    return true;
}

bool Variable::addSuccessor(const string &var)
{
    if(!domain->contains(var))
        return false;

    Variable *tmpvar = domain->getPar(var);

    if(tmpvar->getType()!=OBJECTIVEFUNCTIONVARIABLE)
        return false;

    successors.insert(var);
    return true;
}

bool Variable::removeSuccessor(const string &var)
{
    if(successors.find(var)==successors.end())
        return false;

    successors.erase(successors.find(var));
    return true;
}

void Variable::fireUpdate()
{
    BOOST_FOREACH(string var ,successors)
    {
        ObjectiveFunctionVariable* variable = static_cast<ObjectiveFunctionVariable*>(domain->getPar(var));
        variable->fireUpdate();
    }
}

string Variable::getName() const
{
    return name;
}

VARTYPE Variable::getType() const
{
    return type;
}

void Variable::setDomain(Domain* dom)
{
    domain=dom;
}
