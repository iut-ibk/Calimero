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
#include <DummyModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>
#include <QString>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(DummyModel)

DummyModel::DummyModel()
{
    setDataType("fac n",UINT,"0");
    setDataType("iterations",UINT,"1000");
}

int DummyModel::fac(int x) {
    if (x < 2)
    {
        return x;
    }
    return x*fac(x-1);
}

bool DummyModel::exec(Domain *dom)
{
    //extract result container
    if(!dom->getAllPars(ITERATIONVARIABLE).size())
        return false;

    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);

    //start calculation
    vector<double> result;

    int facn = QString::fromStdString(getValueOfParameter("fac n")).toInt();
    int iterations = QString::fromStdString(getValueOfParameter("iterations")).toInt();

    int tmp = 0;

    for(int index=0; index < iterations; index++)
        tmp=fac(facn);


    result.push_back(tmp+1);
    iterationresult->setValues(result);

    return true;
}
