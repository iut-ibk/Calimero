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
#include <ModelSimThreadPool.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IModelSimulator.h>
#include <IterationResult.h>
#include <Logger.h>
#include <ExternalParameterRegistry.h>
#include <Domain.h>
#include <ModelSimRunnable.h>
#include <Calibration.h>
#include <CalibrationEnv.h>

ModelSimThreadPool::ModelSimThreadPool(int threadnum,bool disableautothreads)
{
    this->disableautothreads=disableautothreads;

    if(disableautothreads)
        this->setMaxThreadCount(0);
    else
        this->setMaxThreadCount(threadnum);
}

ModelSimThreadPool::~ModelSimThreadPool()
{
}

bool ModelSimThreadPool::pushIteration( vector<CalibrationVariable*> vars, Calibration *calibration)
{
    QMutexLocker *locker = new QMutexLocker(&mutex);
    ModelSimRunnable *simulation = new ModelSimRunnable(vars,calibration);
    delete locker;
    simulation->setAutoDelete(true);

    if(disableautothreads)
    {
        if(!CalibrationEnv::getInstance()->isCalibrationRunning())
        {
            delete simulation;
            return false;
        }

        simulation->run();
        delete simulation;
        return true;
    }
    else
    {
        start(simulation);
    }
    return true;
}
