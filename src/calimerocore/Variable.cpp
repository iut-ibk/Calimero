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
    set<string> cpsucc = successors;
    BOOST_FOREACH( string var ,cpsucc )
            static_cast<ObjectiveFunctionVariable*>(domain->getPar(var))->removeParameter(name);

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
