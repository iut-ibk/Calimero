#include <Domain.h>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>

Domain::Domain(){
}

Domain::~Domain()
{
    std::pair<string,Variable*> p;
    BOOST_FOREACH(p,members)
            delete p.second;
}

void Domain::setPar(Variable* var)
{
    if(contains(var->getName()))
        delete members[var->getName()];

    members[var->getName()]=var;
    var->setDomain(this);
}

Variable* Domain::getPar(const string &name)
{
    if(members.find(name)!=members.end())
            return members[name];

    return 0;
}

bool Domain::contains(string var)
{
    if(!getPar(var))
        return false;

    return true;
}
