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
#ifndef LOGSINK_H
#define LOGSINK_H

#include <string>
#include <ostream>
#include <CalimeroGlob.h>
#include <QMutex>
#include <QMutexLocker>

class LSEndl {};

class CALIMERO_PUBLIC LogSink
{
public:
        virtual ~LogSink() {}
        virtual void close() = 0;
        virtual LogSink &operator<<(const std::string &string) = 0;
        virtual LogSink &operator<<(const char *string) = 0;
        virtual LogSink &operator<<(int i) = 0;
        virtual LogSink &operator<<(LSEndl) = 0;
};

class CALIMERO_PUBLIC OStreamLogSink : public LogSink {
public:
        OStreamLogSink(std::ostream &ostream);
        ~OStreamLogSink();
        void close() {}
        LogSink &operator<<(const std::string &string);
        LogSink &operator<<(const char *string);
        LogSink &operator<<(int i);
        LogSink &operator<<(LSEndl e);

private:
        std::ostream &out;
        QMutex *mutex;
};

#endif // LOGSINK_H
