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
#include <NativeErrorSquare.h>
#include <vector>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>

CALIMERO_DECLARE_OFUNCTION_NAME(NativeErrorSquare)

NativeErrorSquare::NativeErrorSquare(){}

NativeErrorSquare::~NativeErrorSquare(){}

std::vector<double> NativeErrorSquare::eval(std::vector<Variable*> iterationparameters,
                             std::vector<Variable*> observedparameters,
                             std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)

{
    std::vector<double> result;

    //extract vectors
    std::vector<std::vector<double> > v;

    for (uint index=0; index < iterationparameters.size(); index++)
        v.push_back(iterationparameters.at(index)->getValues());

    for (uint index=0; index<observedparameters.size(); index++)
        v.push_back(observedparameters.at(index)->getValues());

    for (uint index=0; index<objectivefunctionparameters.size(); index++)
        v.push_back(objectivefunctionparameters.at(index)->getValues());

    //check number of vectors
    if(v.size()!=2)
    {
        Logger(Error) << "Only two parameters are allowed in NativeErrorSquare function";
        result.push_back(999999999);
        return result;
    }

    //check length of vectors
    if(v[0].size() != v[1].size())
    {
        Logger(Error) << "Length of vectors must be equal";
        result.push_back(999999999);
        return result;
    }

    //calculate result vector
    for(uint index=0; index < v.at(0).size(); index++)
    {
        double v1 = v[0][index];
        double v2 = v[1][index];

        result.push_back((v1-v2)*(v1-v2));
    }

    return result;
}

CALIMERO_DECLARE_OFUNCTION_NAME(SSE)

SSE::SSE()
{
}

std::vector<double> SSE::eval(std::vector<Variable*> iterationparameters,
                                 std::vector<Variable*> observedparameters,
                                 std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{
    NativeErrorSquare error;
    std::vector<double> ev = error.eval(iterationparameters,observedparameters,objectivefunctionparameters);

    double result = 0.0;

    for(uint index=0; index < ev.size(); index++)
        result += ev[index];

    return std::vector<double>(1,result);
}

CALIMERO_DECLARE_OFUNCTION_NAME(MSE)

MSE::MSE()
{
}

std::vector<double> MSE::eval(std::vector<Variable*> iterationparameters,
                                 std::vector<Variable*> observedparameters,
                                 std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{
    NativeErrorSquare error;
    std::vector<double> ev = error.eval(iterationparameters,observedparameters,objectivefunctionparameters);

    double result = 0.0;

    for(uint index=0; index < ev.size(); index++)
        result += ev[index];

    result /= ev.size();

    return std::vector<double>(1,result);
}
