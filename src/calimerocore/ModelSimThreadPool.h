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
#ifndef MODELSIMTHREADPOOL_H
#define MODELSIMTHREADPOOL_H

#include <vector>
#include <QThreadPool>
#include <CalimeroGlob.h>
#include <string>
#include <set>
#include <QMutex>
#include <QMutexLocker>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IModelSimulator;
class IterationResult;
class Domain;
class ExternalParameterRegistry;
class Calibration;

using namespace std;

class CALIMERO_PUBLIC ModelSimThreadPool : public QThreadPool
{
private:
    QMutex mutex;
    bool disableautothreads;
public:
    ModelSimThreadPool(int threadnum,bool omp);
    ~ModelSimThreadPool();
    bool pushIteration( vector<CalibrationVariable*> vars, Calibration *calibration);
};

class CalimeroSleep : public QThread
{
public:
    static void msleep(int ms)
    {
        QThread::msleep(ms);
    }
};

#endif // MODELSIMTHREADPOOL_H
