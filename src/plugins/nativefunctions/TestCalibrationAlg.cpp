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
#include <TestCalibrationAlg.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(TestCalibrationAlg)

TestCalibrationAlg::TestCalibrationAlg()
{
    setDataType("parallel",UINT,"1");
    setDataType("clean results", BOOL, "1");
}


TestCalibrationAlg::~TestCalibrationAlg()
{
}

bool TestCalibrationAlg::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    Logger(Debug) << "start called in native class \"TestCalibrationAlg\"";

    CalibrationVariable *pi = calibrationpars[0];

    for(double index=pi->getMin(); index<pi->getMax();index=index+pi->getStep())
    {
        vector<double> result = pi->getValues();
        result[0]=index;
        pi->setValues(result);
        if(!env->execIteration(calibrationpars))
            return true;
    }

    return true;
}
