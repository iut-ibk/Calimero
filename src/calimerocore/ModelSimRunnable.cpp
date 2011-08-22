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
#include <ModelSimRunnable.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IterationResult.h>
#include <IModelSimulator.h>
#include <CalibrationEnv.h>
#include <IterationResult.h>
#include <QRegExp>
#include <Exception.h>

ModelSimRunnable::ModelSimRunnable(vector<CalibrationVariable*> newcalpars,
                                   Calibration *calibration)
{
    error=false;
    //clone all
    dom = new Domain(*(calibration->getDomain()));
    externalfilehandler = new ExternalParameterRegistry(*(calibration->getExternalParameterRegistry()));

    //create result container and save all calibration parameters
    result = calibration->newIterationResult();
    this->calibration = calibration;

    //add calibration parameters to new domain
    BOOST_FOREACH(CalibrationVariable *var, newcalpars)
        dom->setPar(var);

    vector<string> templatenames = externalfilehandler->getAllTemplateNames();

    //create instance of model simulator
    sim = CalibrationEnv::getInstance()->getModelSimulatorReg()->getFunction(calibration->getModelSimulator());

    //convert each modelsimulator setting
    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getModelSimulatorSettings())
    {
        QString setting = QString::fromStdString(p.second);

        //each template name to path
        BOOST_FOREACH(string name, templatenames)
        {
            QString path = QString::fromStdString(externalfilehandler->getPath(name));
            path.replace(QRegExp("\\$iteration\\$"), QString::number(result->getIterationNumber()));
            setting.replace(QRegExp(QString::fromStdString(name)), path);
        }


        if(!sim->setValueOfParameter(p.first,setting.replace(QRegExp("\\$iteration\\$"), QString::number(result->getIterationNumber())).toStdString()))
        {
            error=true;
            return;
        }
    }
}

ModelSimRunnable::~ModelSimRunnable()
{
    if(sim)
        delete sim;

    if(dom)
        delete dom;

    if(externalfilehandler)
        delete externalfilehandler;
}

void ModelSimRunnable::run()
{
    if(CalibrationEnv::getInstance()->calibrationShutDownState())
        return;

    if(error)
    {
        Logger(Error) << "Could not execute \"Model\"";
        CalibrationEnv::getInstance()->stopCalibration();
        return;
    }

    //create all external files
    if(!externalfilehandler->createValueFiles(this->dom,result->getIterationNumber()))
        return;

    //run simulator
    if(!sim)
        return;

    try
    {
        if(!sim->exec(dom))
        {
            Logger(Error) << "Could not execute \"Model\"";
            CalibrationEnv::getInstance()->stopCalibration();
        }
    }
    catch (CalimeroException e)
    {
        Logger(Error) << e.exceptionmsg;
        Logger(Error) << "Could not execute \"Model\"";
        CalibrationEnv::getInstance()->stopCalibration();
    }

    //extract all files
    if(!externalfilehandler->updateParameters(dom,result->getIterationNumber()))
    {
        Logger(Error) << "Could not extract result files";
        CalibrationEnv::getInstance()->stopCalibration();
        return;
    }

    Logger(Debug) << "Extract results in ModelSimRunnable";
    //save values in result container
    result->setResults(dom);
    return;
}
