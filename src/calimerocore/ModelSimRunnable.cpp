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

        if(!sim->setValueOfParameter(p.first,setting.toStdString()))
        {
            delete sim;
            delete dom;
            delete externalfilehandler;
            sim = 0;
        }
    }
}

ModelSimRunnable::~ModelSimRunnable()
{
    Logger(Debug) << "ModelSimRunnable destroyed";
}

void ModelSimRunnable::run()
{
    //create all external files
    if(!externalfilehandler->createValueFiles(this->dom,result->getIterationNumber()))
    {
        delete sim;
        delete dom;
        delete externalfilehandler;
        return;
    }

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

    //clean sim
    delete sim;

    //extract all files
    if(!externalfilehandler->updateParameters(dom,result->getIterationNumber()))
    {
        Logger(Error) << "Could not extract result files";
        CalibrationEnv::getInstance()->stopCalibration();
        delete dom;
        delete externalfilehandler;
        return;
    }

    Logger(Debug) << "Extract results in ModelSimRunnable";
    //save values in result container
    result->setResults(dom);

    //clean thread
    delete dom;
    delete externalfilehandler;
}
