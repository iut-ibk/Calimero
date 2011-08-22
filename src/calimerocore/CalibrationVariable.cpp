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
#include <CalibrationVariable.h>
#include <string>
#include <vector>
#include <list>
#include <boost/foreach.hpp>

using namespace std;

CalibrationVariable::CalibrationVariable(string Name, vector<double> values) : Variable(Name, values, CALIBRATIONVARIABLE)
{
    initvalues=values;
    values=values;

    min = -10000000000;
    max = 10000000000;
    step = 0.00001;
}


CalibrationVariable::CalibrationVariable(const CalibrationVariable &oldvar) : Variable(oldvar)
{
    initvalues=oldvar.initvalues;
    min = oldvar.min;
    max = oldvar.max;
    step = oldvar.step;
}

CalibrationVariable::~CalibrationVariable()
{}

vector<double> CalibrationVariable::getInitValues()
{
    return initvalues;
}

void CalibrationVariable::setInitValues(vector<double> value)
{
    if(!checkrange(value,min,max))
        return;

    initvalues.clear();
    initvalues=value;
}

bool CalibrationVariable::setValues(vector<double> value)
{
    if(!checkrange(value,min,max))
        return false;

    return Variable::setValues(value);
}

void CalibrationVariable::setMax(double value)
{
    //if(value==max || value<min)
    //    return;

    max=value;
}

double CalibrationVariable::getMax()
{
    return max;
}

void CalibrationVariable::setMin(double value)
{
    //if(value==min || value>max)
    //    return;

    min=value;
}

double CalibrationVariable::getMin()
{

    return min;
}

bool CalibrationVariable::checkrange(const vector<double> vec, double min, double max)
{
    /*
    BOOST_FOREACH(double value, vec)
    {
       if(value < min || value > max)
           return false;
   }
    */
   return true;
}

double CalibrationVariable::getStep()
{
    return step;
}

void CalibrationVariable::setStep(double step)
{
    this->step=step;
}

