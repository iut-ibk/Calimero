#include <Domain.h>
#include <Logger.h>
#include <boost/foreach.hpp>
#include <Variable.h>

Domain::Domain(string name, DOMAINTYPE type)
{
    superdomain=0;
    this->name=name;
    this->type=type;
}

Domain::~Domain()
{
    if(superdomain!=0)
        superdomain->removeSubDomain(this);

    std::pair<string,Domain*> p;
    BOOST_FOREACH(p,members)
            delete p.second;
}

string Domain::getName() const
{
    return name;
}

Domain::DOMAINTYPE Domain::getType() const
{
    return type;
}

void Domain::setSuperDomain(Domain *super)
{
    if(!super->addSubDomain(this))
        Logger(Error) << "Domain conflict with [" << getName() << "] in [" << super->getName() << "]";
}

bool Domain::setParameter(Variable* var)
{
    return addSubDomain(var);
}

Variable* Domain::getParameter(const string &name)
{
    if(members.find(name)!=members.end())
        if(members[name]->getType() < 4)
            return (Variable*)members[name];

    if(superdomain!=0)
        return superdomain->getParameter(name);

    return 0;
}

bool Domain::contains(string var)
{
    if(!getParameter(var))
        return false;

    return true;
}

bool Domain::addSubDomain(Domain *sub)
{
    if(members.find(sub->getName())==members.end())
        return false;

    members[sub->getName()]=sub;
    return true;
}

void Domain::removeSubDomain(Domain *sub)
{
    if(members.find(sub->getName())!=members.end())
        members.erase(members.find(sub->getName()));
}
