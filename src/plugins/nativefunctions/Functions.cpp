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
//general includes
#include <CalimeroGlob.h>
#include <Registry.h>
#include <IFunctionFactory.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>

//Objective function includes
#include <TestFunction.h>
#include <NativeErrorSquare.h>

//Calibration algorithm includes
#include <TestCalibrationAlg.h>
#include <BruteForceSearch.h>

//Model simulator includes
#include <ExternalModel.h>
#include <TestModel.h>

extern "C"
{
    void CALIMERO_PUBLIC registerObjectiveFunctions(Registry<IObjectiveFunction> *registry)
    {
        registry->registerFunction(new NativeFunctionFactory<TestOFunction>());
        registry->registerFunction(new NativeFunctionFactory<NativeErrorSquare>());
        registry->registerFunction(new NativeFunctionFactory<SSE>());
        registry->registerFunction(new NativeFunctionFactory<MSE>());
    }

    void CALIMERO_PUBLIC registerCalibrationAlgs(Registry<ICalibrationAlg> *registry)
    {
        registry->registerFunction(
                    new NativeFunctionFactory<TestCalibrationAlg>());
        registry->registerFunction(
                    new NativeFunctionFactory<BruteForceSearch>());
    }

    void CALIMERO_PUBLIC registerModelSimulations(Registry<IModelSimulator> *registry)
    {
        registry->registerFunction(
                    new NativeFunctionFactory<ExternalModel>());
        registry->registerFunction(
                    new NativeFunctionFactory<TestModel>());
        registry->registerFunction(
                    new NativeFunctionFactory<VectorModel>());
        registry->registerFunction(
                    new NativeFunctionFactory<Schmutzstoffmodell>());
    }
}
