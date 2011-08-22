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
#ifndef OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
#define OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED

#include <Variable.h>
#include <IObjectiveFunction.h>
#include <string>
#include <QMutex>
#include <QMutexLocker>

class ObjectiveFunctionVariable;
class Domain;
class IObjectiveFunction;

class CALIMERO_PUBLIC ObjectiveFunctionVariable : public Variable
{
    private:
        set<string> iterationparameters;
        set<string> observedparameters;
        set<string> objectivefunctionparameters;
        bool needupdate;
        std::string functionname;
        std::map<std::string, std::string> functionsettings;
        QMutex mutex;

    public:
        ObjectiveFunctionVariable(const ObjectiveFunctionVariable &oldvar);
        ObjectiveFunctionVariable(string Name);
        virtual ~ObjectiveFunctionVariable();
        bool addParameter(const string &var);
        bool removeParameter(const string &var);
        set<string> getIterationParameters();
        set<string> getObservedParameters();
        set<string> getObjectiveFunctionParameters();
        vector<double> getValues();
        virtual void fireUpdate();
        bool setObjectiveFunction(std::string ofunction, map<string,string> settings);
        std::string getObjectiveFunction();
        bool containsParameter(const string &var);
        map<string,string> getObjectiveFunctionSettings();
        bool parameterCycleCheck(string var);

    private:
        bool setValues(vector<double> value);
        bool calc();
};

#endif // OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
