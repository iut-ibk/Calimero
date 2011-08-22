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
#ifndef IFUNCTIONFACTORY_H
#define IFUNCTIONFACTORY_H

#include <string>
#include <CalimeroGlob.h>
#include <IFunction.h>
#include <Logger.h>

class IFunctionFactory {
    public:
        virtual ~IFunctionFactory(){}
        virtual IFunction *createFunction() const = 0;
        virtual std::string getFunctionName() = 0;
};

template <typename T> class NativeFunctionFactory : public IFunctionFactory
{
    public:
        NativeFunctionFactory();
        virtual T *createFunction() const;
        virtual std::string getFunctionName();
};

template <typename T> NativeFunctionFactory<T>::NativeFunctionFactory() {}

template <typename T> T *NativeFunctionFactory<T>::createFunction() const
{
    Logger(Debug) << "createFunction in NativeFunctionFactory called";
    return new T();
}

template <typename T> std::string NativeFunctionFactory<T>::getFunctionName()
{
        return T::name;
}

#endif // IFUNCTIONFACTORY_H
