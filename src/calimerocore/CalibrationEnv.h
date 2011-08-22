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
#ifndef CALIBRATIONENV_H
#define CALIBRATIONENV_H

#include <Registry.h>
#include <IObjectiveFunction.h>
#include <IModelSimulator.h>
#include <ICalibrationAlg.h>
#include <Calibration.h>
#include <QThread>
#include <CalimeroGlob.h>
#include <ModelSimThreadPool.h>
#include <ExternalParameterRegistry.h>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <IResultHandler.h>

class QWidget;

class CALIMERO_PUBLIC CalibrationEnv : public QThread
{
public:
    enum CALIBRATIONSTATE{CALIBRATIONINIT=0, CALIBRATIONRUNNING=1, CALIBRATIONSHUTDOWN=2, CALIBRATIONNOTRUNNING=3};

private:
    int numthread;
    bool inmemory;
    bool stopthread;
    static CalibrationEnv *instance;
    QMutex *mutex;
    QReadWriteLock lock;
    Calibration *calibration;
    ModelSimThreadPool *threadpool;
    Registry<IObjectiveFunction>* oreg;
    Registry<IModelSimulator>* mreg;
    Registry<ICalibrationAlg>* creg;
    Registry<IResultHandler>* rreg;
    CALIBRATIONSTATE calstate;

    void runCalibration();
    CALIBRATIONSTATE getCalibrationState();
    void setCalibrationState(CALIBRATIONSTATE state);

public:
    CalibrationEnv();
    ~CalibrationEnv();
    void run();
    static CalibrationEnv* getInstance();
    Registry<IObjectiveFunction>* getObjectiveFunctionReg();
    Registry<IModelSimulator>* getModelSimulatorReg();
    Registry<ICalibrationAlg>* getCalibrationAlgReg();
    Registry<IResultHandler>* getResultHandlerReg();
    vector<string> getAvailableObjectiveFunctions();
    vector<string> getAvailableModelSimulators();
    vector<string> getAvailableCalibrationAlgs();
    vector<string> getAvailableResultHandlers();

    //calibration-settings
    bool setCalibration(Calibration *cal);
    Calibration* getCalibration();
    bool isCalibrationRunning();
    bool calibrationShutDownState();
    int getNumThreads();
    bool setNumThreads(int num);
    bool inMemory();
    void setInMemory(bool inmemory);

    //calibration
    bool startCalibration();
    void stopCalibration();
    bool execIteration(vector<CalibrationVariable*> calibrationparameters);
    void barrier();

    //result analysis
    void runAllEnabledResultHandler();
    void runResultHandler(string name);
};

#endif // CALIBRATIONENV_H
