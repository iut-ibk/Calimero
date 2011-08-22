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
#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <CalimeroGlob.h>

using namespace std;
class ObjectiveFunctionVariable;
class Domain;

enum  VARTYPE {CALIBRATIONVARIABLE = 0,
               OBJECTIVEFUNCTIONVARIABLE = 1,
               OBSERVEDVARIABLE = 2,
               ITERATIONVARIABLE = 3,
              };

class CALIMERO_PUBLIC Variable
{
    protected:
        set<string> successors;
        vector<double> values;
        string name;
        VARTYPE type;
        Domain* domain;
    public:
        Variable(string Name, vector<double> value, VARTYPE TYPE);
        Variable(const Variable &oldvar);
        virtual ~Variable();
        virtual vector<double> getValues();
        virtual bool setValues(const vector<double> values);
        virtual bool addSuccessor(const string &var);
        virtual bool removeSuccessor(const string &var);
        virtual void fireUpdate();
        string getName() const;
        VARTYPE getType() const;
        void setDomain(Domain* dom);
};

#endif // VARIABLE_H_INCLUDED
