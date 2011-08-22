/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
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
