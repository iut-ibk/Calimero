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
#ifndef ParticleSwarmAlgorithm_H
#define ParticleSwarmAlgorithm_H

#include <ICalibrationAlg.h>
#include <vector>

class CalibrationVariable;
class ObjectiveFunctionVariable;
class CalibrationEnv;
class Calibration;

CALIMERO_DECLARE_CALFUNCTION(ParticleSwarmAlgorithm)

private:
    int maxiteration,swarmsize;
    double maxerror,omega,phig,phip,gbesthealth, clamping;
    vector<double> gbestposition,pbesthealth,vmax;
    vector<vector<double>* > pvelocity,pposition,pbestposition;
    CalibrationEnv *env;
    Calibration *calibration;
    vector<CalibrationVariable*> calibrationpars;
    vector<ObjectiveFunctionVariable*> opars;
    bool initglobalhealth;

public:
    ParticleSwarmAlgorithm();
    virtual ~ParticleSwarmAlgorithm();
    virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
    bool runStep();
    double randval(double low, double high, double step);
    vector<double> vectorAddition(vector<double> a, vector<double> b);
    vector<double> vectorMinus(vector<double> a, vector<double> b);
    vector<double> scalarAddition(vector<double> a, double scalar);
    vector<double> vectorMultiplication(vector<double> a, vector<double> b);
    vector<double> scalarMultiplication(vector<double> a, double scalar);
    vector<double> vmaxCheck(vector<double> velocity);
    vector<double> positionCheck(vector<double> position);
    void clean();
};

#endif // ParticleSwarmAlgorithm_H
