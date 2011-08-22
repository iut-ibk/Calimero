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
#include <BruteForceSearch.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>
#include <math.h>

#if defined(OPENMP_ENABLED)
#include <omp.h>
#endif

CALIMERO_DECLARE_CALFUNCTION_NAME(BruteForceSearch)

BruteForceSearch::BruteForceSearch()
{
    setDataType("parallel",UINT,"1");
    setDataType("clean results", BOOL, "1");
    setDataType("disableautothreads",BOOL,"0");
}


BruteForceSearch::~BruteForceSearch()
{
}

bool BruteForceSearch::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    steps.clear();
    uint maxiterations=1;
    this->env=env;

    //init
    for(uint index=0; index<calibrationpars.size(); index++)
    {
        CalibrationVariable *currentvar = calibrationpars[index];
        steps.push_back(floor((currentvar->getMax()-currentvar->getMin())/currentvar->getStep())+1);
        maxiterations*=steps[index];
    }

    Logger(Standard) << "Maxiterations: " << (int)maxiterations;
    //make samples
    if(QString(getValueOfParameter("disableautothreads").c_str()).toInt())
    {

#if defined(OPENMP_ENABLED)

        omp_set_num_threads(QString(getValueOfParameter("parallel").c_str()).toInt());
        bool ok=true;
        #pragma omp parallel for
        for(int iteration=0; iteration<(int)maxiterations; iteration++)
        {
            if(ok)
                ok=ok && sample(iteration,calibrationpars);
        }
    }
#else
        Logger(Error) << "Please compile calimero with openmp";
        return false;
    }
#endif
    else
    {
        for(uint iteration=0; iteration<maxiterations; iteration++)
        {
            if(!sample(iteration,calibrationpars))
                return false;
        }
    }
    return true;
}

bool BruteForceSearch::sample(int iteration,vector<CalibrationVariable*> calibrationpars)
{
    vector<CalibrationVariable*> newpars;
    int forward = iteration;

    for(uint var=0; var < calibrationpars.size(); var++)
    {
        CalibrationVariable *newpar = new CalibrationVariable(*calibrationpars[var]);
        std::vector<double> value;
        value.push_back(newpar->getMin()+newpar->getStep()*(forward%steps[var]));
        newpar->setValues(value);
        forward = forward/steps[var];
        newpars.push_back(newpar);
    }

    bool ok = env->execIteration(newpars);

    for(uint var=0; var < newpars.size(); var++)
        delete newpars[var];

    if(!ok)
        return false;

    return true;
}
