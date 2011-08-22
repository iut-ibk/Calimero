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
#ifndef LOG_H
#define LOG_H

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <CalimeroGlob.h>

using namespace std;

class Node;
class Simulation;
class Logger;
class LogSink;

enum LogLevel {
        Debug = 0,
        Standard = 1,
        Warning = 2,
        Error = 3
};

#define LOG_HEAD \
        if (level <= max) return *this;\
        out << logLevel() << ": " << "\n"

class CALIMERO_PUBLIC Log
{
public:
        static void init(LogSink *sink, LogLevel max = Debug);
        static void shutDown();
        static Log *getInstance();
        friend class Logger;
private:
        Log();
        virtual ~Log();

        static Log *instance;

        LogSink *sink;
        LogLevel max;
};

#endif // LOG_H
