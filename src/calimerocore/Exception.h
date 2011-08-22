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
#ifndef PYTHONEXCEPTION_H
#define PYTHONEXCEPTION_H

#include <string>
#include <CalimeroGlob.h>

enum  EXCEPTIONTYPE {   CALIMEROEXCEPTION = 0,
                        BOOSTPYTHONEXCEPTION = 1
              };

class CALIMERO_PUBLIC CalimeroException{
public:
    EXCEPTIONTYPE exceptiontype;
    std::string exceptionmsg;

    CalimeroException()
    {
        exceptiontype=CALIMEROEXCEPTION;
        exceptionmsg="Default calimero exception";
    }

    CalimeroException(std::string exceptionmsg)
    {
        this->exceptiontype=CALIMEROEXCEPTION;
        this->exceptionmsg=exceptionmsg;
    }
};

class CALIMERO_PUBLIC PythonException : public CalimeroException {
public:
        PythonException(std::string error, std::string msg);
        std::string type;
        std::string value;
        std::string traceback;
};
#endif // PYTHONEXCEPTION_H
