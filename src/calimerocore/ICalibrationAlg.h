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
#ifndef ICALIBRATIONALG_H
#define ICALIBRATIONALG_H

#include <IFunction.h>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>


class CalibrationEnv;
class Calibration;

#define CALIMERO_DECLARE_CALFUNCTION(function)  \
class CALIMERO_PUBLIC function : public ICalibrationAlg { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_CALFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class CALIMERO_PUBLIC ICalibrationAlg : public IFunction
{
    public:
        static IFUNCTIONTYPE getType()
        {
            return CALIBRATIONALGORITHM;
        };

        ICalibrationAlg(){};
        virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration) = 0;
};
#endif // ICALIBRATIONALG_H
