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
#include <TestModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>
#include <QString>
#include <math.h>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(Schmutzstoffmodell)

Schmutzstoffmodell::Schmutzstoffmodell()
{
    setDataType("Abfluss-Parameter",STRING,"");
    setDataType("W-Parameter",STRING,"");
    setDataType("b-Parameter",STRING,"");
    setDataType("Result-Parameter",STRING,"");
}

bool Schmutzstoffmodell::exec(Domain *dom)
{
    std::vector<double> result;
    std::string abfluss = QString::fromStdString(getValueOfParameter("Abfluss-Parameter")).toStdString();
    std::string wpar = QString::fromStdString(getValueOfParameter("W-Parameter")).toStdString();
    std::string bpar = QString::fromStdString(getValueOfParameter("b-Parameter")).toStdString();
    std::string rpar = QString::fromStdString(getValueOfParameter("Result-Parameter")).toStdString();

    if(!dom->contains(abfluss))
        return false;

    if(!dom->contains(wpar))
        return false;

    if(!dom->contains(bpar))
        return false;

    if(!dom->contains(rpar))
        return false;

    if(dom->getPar(wpar)->getType()!=CALIBRATIONVARIABLE)
        return false;

    if(dom->getPar(bpar)->getType()!=CALIBRATIONVARIABLE)
        return false;

    if(dom->getPar(rpar)->getType()!=ITERATIONVARIABLE)
        return false;

    if(dom->getPar(abfluss)->getType()!=OBSERVEDVARIABLE)
        return false;

    CalibrationVariable *w = static_cast<CalibrationVariable*>(dom->getPar(wpar));
    CalibrationVariable *b = static_cast<CalibrationVariable*>(dom->getPar(bpar));
    Variable *r = dom->getPar(rpar);
    Variable *a = dom->getPar(abfluss);

    std::vector<double> avector = a->getValues();

    for(uint index=0; index < avector.size(); index++)
        result.push_back(w->getValues()[0]*powf(avector[index],b->getValues()[0]));

    return r->setValues(result);
}


CALIMERO_DECLARE_MODELSIMULATOR_NAME(TestModel)

TestModel::TestModel(){}

bool TestModel::exec(Domain *dom)
{
    CalibrationVariable* pi = static_cast<CalibrationVariable*>(dom->getAllPars(CALIBRATIONVARIABLE).at(0));
    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);
    vector<double> result = iterationresult->getValues();
    result[0]=pi->getValues()[0]*5;
    iterationresult->setValues(result);
    return true;
}

CALIMERO_DECLARE_MODELSIMULATOR_NAME(VectorModel)

VectorModel::VectorModel()
{
    setDataType("wait-time",UINT,"0");
}

bool VectorModel::exec(Domain *dom)
{
    //extract calibration parameters
    if(dom->getAllPars(CALIBRATIONVARIABLE).size() != 2)
        return false;

    double value = 0.0;
    double size = 0.0;

    for(int index = 0; index < 2; index++)
    {
        CalibrationVariable* currentparameter = static_cast<CalibrationVariable*>(dom->getAllPars(CALIBRATIONVARIABLE).at(index));
        vector<double> currentvector = currentparameter->getValues();
        if(!currentvector.size())
            return false;

        double currentvalue = currentvector[0];
        if(currentparameter->getName()=="value")
            value = currentvalue;
        else
            size = currentvalue;
    }

    //extract result container
    if(!dom->getAllPars(ITERATIONVARIABLE).size())
        return false;

    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);

    //start calculation
    vector<double> result;

    for(int index = 0; index < size; index++)
        result.push_back((index+1)*(index+1)*value);

    iterationresult->setValues(result);

    int waittime = QString::fromStdString(getValueOfParameter("wait-time")).toInt();

    if(!waittime)
        return true;

    double resultv = 0.0;

    for(int index=0; index<waittime; index++)
        resultv = fac(index);

    return true;
}

double fac(int value)
{
    if(value==0)
        return 0;

    double result = 1;

    for(int index=1; index <= value; index++)
        result*=index;

    return result;
}
