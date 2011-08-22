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
#include <ExternalModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>
#include <CalibrationEnv.h>
#include <QProcess>
#include <Logger.h>
#include <QDir>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(ExternalModel)

ExternalModel::ExternalModel()
{
    setDataType("Timeout",INT,"-1");

    setDataType("Pre-Workspace",DIRSTRING,"");
    setDataType("Pre-exec-path",FILESTRING,"");
    setDataType("Pre-arguments",STRING,"");

    setDataType("Iteration-Workspace",DIRSTRING,"");
    setDataType("Iteration-exec-path", FILESTRING,"");
    setDataType("Iteration-arguments",STRING,"");

    setDataType("Post-Workspace",DIRSTRING,"");
    setDataType("Post-exec-path",FILESTRING,"");
    setDataType("Post-arguments",STRING,"");
}

QStringList ExternalModel::convertArgumentList(QString string)
{
    QString news = string.replace("\\ ","\\$");
    QStringList tmplist = news.split(" ");
    QStringList result;
    for(int index=0; index<tmplist.size(); index++)
        result.append(tmplist[index].replace("\\$"," "));

    return result;
}

bool ExternalModel::exec(Domain *dom)
{
    int timeout = QString::fromStdString(getValueOfParameter("Timeout")).toInt();

    QString iterationworkspace = QString::fromStdString(getValueOfParameter("Iteration-Workspace"));
    QString preworkspace = QString::fromStdString(getValueOfParameter("Pre-Workspace"));
    QString postworkspace = QString::fromStdString(getValueOfParameter("Post-Workspace"));

    QString preprog = QString::fromStdString(getValueOfParameter("Pre-exec-path"));
    QString iterationprog = QString::fromStdString(getValueOfParameter("Iteration-exec-path"));
    QString postprog = QString::fromStdString(getValueOfParameter("Post-exec-path"));

    QStringList preprogarguments = convertArgumentList(QString::fromStdString(getValueOfParameter("Pre-arguments")));
    QStringList iterationprogarguments = convertArgumentList(QString::fromStdString(getValueOfParameter("Iteration-arguments")));
    QStringList postprogarguments = convertArgumentList(QString::fromStdString(getValueOfParameter("Post-arguments")));

    QProcess process;
    process.setStandardErrorFile(QDir::tempPath()+"/calimero.err");
    process.setStandardOutputFile(QDir::tempPath()+"/calimero.out");

    QDir dir(preworkspace);

    if(preprog!="" && !dir.exists(preprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    dir.setPath(iterationworkspace);

    if(!dir.exists(iterationprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    dir.setPath(postworkspace);

    if(postprog!="" && !dir.exists(postprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    if(preprog!="")
    {
        process.setWorkingDirectory(preworkspace);
        process.start(preprog,preprogarguments);
        process.waitForStarted();

        if(!process.waitForFinished(timeout))
        {
            CalibrationEnv::getInstance()->stopCalibration();
            Logger(Error) << "Not able to start external pre - program";
            return false;
        }
    }

    process.setWorkingDirectory(iterationworkspace);
    process.start(iterationprog,iterationprogarguments);
    process.waitForStarted();

    if(!process.waitForFinished(timeout))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Timeout of external iteration - program";
        return false;
    }

    if(postprog!="")
    {
        process.setWorkingDirectory(postworkspace);
        process.start(postprog,postprogarguments);
        process.waitForStarted();

        if(!process.waitForFinished(timeout))
        {
            CalibrationEnv::getInstance()->stopCalibration();
            Logger(Error) << "Not able to start external post - program";
            return false;
        }
    }
    return true;
}
