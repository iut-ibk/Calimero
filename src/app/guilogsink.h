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
#ifndef GUILOGSINK_H
#define GUILOGSINK_H

#include <LogSink.h>
#include <QObject>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class GuiLogSink : public QObject, public LogSink {
	Q_OBJECT
public:
        QMutex *mutex;
	GuiLogSink();
	virtual ~GuiLogSink();

	void close() {};

	LogSink &operator<<(const std::string &string);
	LogSink &operator<<(const char *string);
	LogSink &operator<<(int i);
	LogSink &operator<<(LSEndl i);

Q_SIGNALS:
	void newLogLine(const QString &line);
private:
	QString buf;
};

#endif // GUILOGSINK_H
