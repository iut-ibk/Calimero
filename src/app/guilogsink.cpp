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
#include "guilogsink.h"
#include <QPlainTextEdit>
#include <sstream>
#include <Log.h>

GuiLogSink::GuiLogSink() {
    mutex = new QMutex(QMutex::Recursive);
}

GuiLogSink::~GuiLogSink() {
    delete mutex;
}

LogSink &GuiLogSink::operator<<(const std::string &string) {
        QMutexLocker locker(mutex);
	buf += QString::fromStdString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(const char *string) {
        QMutexLocker locker(mutex);
	buf += QString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(int i) {
        QMutexLocker locker(mutex);
	buf += QString("%0").arg(i);
	return *this;
}

LogSink &GuiLogSink::operator<<(LSEndl i) {
        QMutexLocker locker(mutex);
	Q_EMIT newLogLine(buf);
	buf.clear();
	return *this;
}
