#include <ModelSimRunnable.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IterationResult.h>
#include <IModelSimulator.h>
#include <CalibrationEnv.h>
#include <IterationResult.h>
#include <QRegExp>

ModelSimRunnable::ModelSimRunnable(vector<CalibrationVariable*> newcalpars,
                                   Calibration *calibration)
{
    this->newcalpars=newcalpars;
    this->calibration=calibration;
}

ModelSimRunnable::~ModelSimRunnable()
{

}

void ModelSimRunnable::run()
{
    dom = new Domain(*(calibration->getDomain()));
    externalfilehandler = new ExternalParameterRegistry(*(calibration->getExternalParameterRegistry()));

    //create result container and save all calibration parameters
    result = calibration->newIterationResult();

    //add calibration parameters to new domain
    BOOST_FOREACH(CalibrationVariable *var, this->newcalpars)
    {
            dom->setPar(new CalibrationVariable(*var));
            delete var;
    }

    //create instance of model simulator
    IModelSimulator *sim = CalibrationEnv::getInstance()->getModelSimulatorReg()->getFunction(calibration->getModelSimulator());

    std::pair<string,string> p;
    vector<string> templatenames = externalfilehandler->getAllTemplateNames();

    //convert each modelsimulator setting
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
            return;
        }
    }

    //create all external files
    if(!externalfilehandler->createValueFiles(this->dom,result->getIterationNumber()))
    {
        delete sim;
        delete dom;
        delete externalfilehandler;
        return;
    }

    //run simulator
    if(!sim->exec(dom))
        CalibrationEnv::getInstance()->stopCalibration();

    //clean sim
    delete sim;

    //extract all files
    if(!externalfilehandler->updateParameters(dom,result->getIterationNumber()))
    {
        delete dom;
        delete externalfilehandler;
        return;
    }

    //save values in result container
    result->setResults(dom);

    //clean thread
    delete dom;
    delete externalfilehandler;
}
