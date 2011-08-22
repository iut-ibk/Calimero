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
#include <Logger.h>
#include <QDateTime>
#include <QString>
#include <LogSink.h>
#include <Variable.h>

Logger::Logger(LogLevel level)
 : sink(*Log::getInstance()->sink), level(level) {
        dirty = false;
        this->max = Log::getInstance()->max;
        if (level >= max) {
                sink << logLevel() << " " << date() << "|";
                dirty = true;
        }
}

Logger::~Logger() {
        if (dirty)
                sink << LSEndl();
}

Logger &Logger::operator <<(LogLevel new_level) {
        level = new_level;
        sink << "\n" << logLevel() << " " << date() << "|";
        dirty = true;
        return *this;
}

Logger &Logger::operator <<(const Variable *var) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }

        switch(var->getType())
        {
        case CALIBRATIONVARIABLE:
            sink << " CalibrationParameter[" << var->getName() << "] ";
            break;
        case OBJECTIVEFUNCTIONVARIABLE:
            sink << " ObjectiveFunctionParameter[" << var->getName() << "] ";
            break;
        case OBSERVEDVARIABLE:
            sink << " ObservedParameter[" << var->getName() << "] ";
            break;
        case ITERATIONVARIABLE:
            sink << " IterationParameter[" << var->getName() << "] ";
            break;
        }

        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const char* s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << s;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const int i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const size_t i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const long i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const double f) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << f;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const float f) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << f;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const string &s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << s;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const QString &s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
                return *this;
        }
        sink << " " << s.toStdString();
        dirty = true;
        return *this;
}


string Logger::logLevel() const {
        switch (level) {
                case Debug:
                        return "DEBUG";
                case Warning:
                        return "WARN";
                case Standard:
                        return "STANDARD";
                case Error:
                        return "ERROR";
        }
        return "UNKNOWN";
}

string Logger::date() const {
        return QDateTime::currentDateTime().toString().toStdString();
}

