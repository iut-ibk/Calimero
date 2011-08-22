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
#ifndef IMODELSIMULATOR_H
#define IMODELSIMULATOR_H

#include <vector>
#include <IFunction.h>
#include <CalimeroGlob.h>
#include <Logger.h>

class CalibrationVariable;
class ObjectiveFunctionVariable;
class Variable;
class IterationResult;
class Domain;

using namespace std;

#define CALIMERO_DECLARE_MODELSIMULATOR(function)  \
class CALIMERO_PUBLIC function : public IModelSimulator { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_MODELSIMULATOR_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }


class CALIMERO_PUBLIC IModelSimulator : public IFunction
{
    public:
        IModelSimulator(){};
        virtual ~IModelSimulator(){};
        static IFUNCTIONTYPE getType()
        {
            return MODELSIMULATOR;
        }


        virtual bool exec(Domain *dom) = 0;
};

#endif // IMODELSIMULATOR_H
