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
#include <FunctionLoader.h>
#include <QDir>
#include <iostream>
#include <CalibrationEnv.h>

void FunctionLoader::loadNative(const string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString file = fileInfo.absoluteFilePath();

        if(!(file.contains(".dll") || file.contains(".so") || file.contains(".dylib") ))
            continue;

        if(file.contains("manifest"))
            continue;

        if(!CalibrationEnv::getInstance()->getCalibrationAlgReg()->addNativePlugin(file.toStdString()))
            continue;
        if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->addNativePlugin(file.toStdString()))
            continue;
        if(!CalibrationEnv::getInstance()->getModelSimulatorReg()->addNativePlugin(file.toStdString()))
            continue;
        if(!CalibrationEnv::getInstance()->getResultHandlerReg()->addNativePlugin(file.toStdString()))
            continue;
    }
}
