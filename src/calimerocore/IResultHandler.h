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
#ifndef IRESULTHANDLER_H
#define IRESULTHANDLER_H

#include <IFunction.h>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <map>
#include <IterationResult.h>

#define CALIMERO_DECLARE_RESULTFUNCTION(function)  \
class CALIMERO_PUBLIC function : public IResultHandler { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_RESULTFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class CALIMERO_PUBLIC IResultHandler : public IFunction
{
    public:
        static IFUNCTIONTYPE getType()
        {
            return RESULTFUNCTION;
        };

        IResultHandler(){};
        virtual bool run(std::vector<IterationResult * > iterationresults) = 0;
        bool test(){Logger(Error) << "test"; return false;};
};

#endif // IRESULTHANDLER_H
