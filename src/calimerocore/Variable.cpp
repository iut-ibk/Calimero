#include "Variable.h"
#include "ObjectiveFunctionVariable.h"
#include <string>
#include <vector>
#include <list>
#include <boost/foreach.hpp>

using namespace std;

Variable::Variable(string Name, vector<double> value, VARTYPE TYPE)
{
    name = Name;
    values = value;
    type = TYPE;
}

Variable::~Variable()
{
    BOOST_FOREACH( ObjectiveFunctionVariable* variable ,successors )
            variable->removeParameter(this);
}

string Variable::getName() const
{
    return name;
}

void Variable::setName(string name)
{
    this->name=name;
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

Variable::VARTYPE Variable::getType() const
{
    return type;
}

bool Variable::addSuccessor(ObjectiveFunctionVariable *var)
{
    if(var==NULL)
        return false;

    successors.insert(var);

    return true;
}

bool Variable::removeSuccessor(ObjectiveFunctionVariable *var)
{
    if(var==NULL)
        return false;

    successors.erase(successors.find(var));;
    return true;
}

void Variable::fireUpdate()
{
    BOOST_FOREACH(ObjectiveFunctionVariable* variable ,successors)
    {
        variable->fireUpdate();
    }
}
