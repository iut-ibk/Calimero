#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <Domain.h>
#include <string>
#include <vector>
#include <list>
#include <boost/foreach.hpp>
#include <assert.h>

using namespace std;

Variable::Variable(string Name, vector<double> value, Domain::DOMAINTYPE TYPE) : Domain(Name,TYPE)
{
    assert(TYPE <= 3);
    values = value;
}

Variable::~Variable()
{
    BOOST_FOREACH( ObjectiveFunctionVariable* variable ,successors )
            variable->removeParameter(this);
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
