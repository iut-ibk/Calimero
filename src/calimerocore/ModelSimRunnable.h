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
#ifndef MODELSIMRUNNABLE_H
#define MODELSIMRUNNABLE_H

#include <QRunnable>
#include <vector>
#include <CalimeroGlob.h>
#include <string>
#include <boost/shared_ptr.hpp>

class CalibrationVariable;
class Variable;
class ObjectiveFunctionVariable;
class IterationResult;
class IModelSimulator;
class Domain;
class ExternalParameterRegistry;
class IterationResult;
class Calibration;

using namespace std;
using namespace boost;

class CALIMERO_PUBLIC ModelSimRunnable : public QRunnable
{
private:
    vector<string> calibrationparameters;
    vector<string> objectivefunctionparameters;
    vector<string>  iterationparameters;
    vector<string> observedparameters;
    Domain *dom;
    ExternalParameterRegistry *externalfilehandler;
    IterationResult *  result;
    Calibration *calibration;
    IModelSimulator *sim;
    bool error;

public:
    ModelSimRunnable(vector<CalibrationVariable*> newcalpars,Calibration *calibration);
    virtual ~ModelSimRunnable();
    void run();
};

#endif // MODELSIMRUNNABLE_H
