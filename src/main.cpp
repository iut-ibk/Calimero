#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <IObjectiveFunction.h>
#include <boost/python.hpp>
#include <PyFunctionFactory.cpp>
#include <PyEnv.h>
#include <Registry.h>
#include <Log.h>
#include <LogSink.h>
#include <Calibration.h>
#include <IModelSimulator.h>
#include <CalibrationEnv.h>

int main()
{
    using namespace boost::python;

    //Logging test
    ostream *out = &cout;
    Log::init(new OStreamLogSink(*out),Standard);
    Logger() << "starting";

    //Calibration init
    CalibrationEnv *cal = CalibrationEnv::getInstance();


    //Python test
    PyEnv *env = PyEnv::getInstance();
    env->addPythonPath("./");
    env->addPythonPath("../../scripts/");
    cal->getObjectiveFunctionReg()->addNativePlugin("calimeronativefunctions.so");
    cal->getCalibrationAlgReg()->addNativePlugin("calimeronativefunctions.so");
    cal->getModelSimulatorReg()->addNativePlugin("calimeronativefunctions.so");
    IObjectiveFunction *testof = cal->getObjectiveFunctionReg()->getFunction("TestOFunction");
    delete testof;

    env->registerFunctions(cal->getObjectiveFunctionReg(),"testcali");
    env->registerFunctions(cal->getObjectiveFunctionReg(),"testcali");
    env->registerFunctions(cal->getCalibrationAlgReg(),"testcalibration");


    //test registry
    if(cal->getObjectiveFunctionReg()->contains("MyOF1"))
        Logger(Debug) << "Registry test DONE";
    else
        Logger(Debug) << "Registry test FAILED";

    //Variable test
    ObjectiveFunctionVariable objectf("var1");
    ObjectiveFunctionVariable objectf2("var2");
    ObjectiveFunctionVariable objectf3("var3");
    ObjectiveFunctionVariable objectf4("var4");
    ObjectiveFunctionVariable objectf5("var5");
    ObjectiveFunctionVariable objectf6("var6");

    if(cal->getObjectiveFunctionReg()->contains("TestOFunction"))
        Logger(Debug) << "Registry test DONE";
    else
        Logger(Debug) << "Registry test FAILED";

    objectf.setObjectiveFunction("TestOFunction",map<string,string>());

    objectf2.addParameter(objectf.getName());
    /*objectf3.addParameter(&objectf2);
    objectf4.addParameter(&objectf3);
    objectf5.addParameter(&objectf4);
    ObjectiveFunctionVariable *pvar = new ObjectiveFunctionVariable("pvar");
    pvar->addParameter(&objectf5);
    objectf6.addParameter(pvar);
    Variable var2("inputvar",vector<double>(4,4),ITERATIONVARIABLE);
    objectf.addParameter(&var2);
    objectf.removeParameter(&var2);
    objectf.getValues();
    objectf.getValues();
    var2.setValues(vector<double>(5,5));
    objectf.getValues();
    objectf5.getValues();
    objectf6.getValues();
    objectf5.getValues();
    objectf6.getValues();
    Variable *nvar = new Variable("nvar",vector<double>(2,2),ITERATIONVARIABLE);
    objectf6.addParameter(nvar);
    objectf.addParameter(nvar);
    delete nvar;
    delete pvar;
    objectf6.getValues();*/


    //calibration test
    Calibration *testcali = new Calibration();
    CalibrationEnv::getInstance()->setCalibration(testcali);
    //cal->startCalibration();
    //CalibrationEnv::getInstance()->getCalibration()->setCalibrationAlg("TestCalibrationAlg",map<string,string>());

    cal->startCalibration();
    while(cal->isCalibrationRunning())
    {
        (void)env;
    }

    CalibrationEnv::getInstance()->getCalibration()->setCalibrationAlg("TestCalibrationAlg",map<string,string>());
    cal->startCalibration();
    while(cal->isCalibrationRunning())
    {
        (void)env;
    }

    CalibrationEnv::getInstance()->getCalibration()->setModelSimulator("ModelSimulatorHandler",map<string,string>());
    cal->startCalibration();
    while(cal->isCalibrationRunning())
    {
        (void)env;
    }



    CalibrationEnv::getInstance()->getCalibration()->addDisabledGroup("test");
    CalibrationEnv::getInstance()->getCalibration()->addParameter(&objectf5);
    CalibrationEnv::getInstance()->getCalibration()->addParameter(&objectf5);
    CalibrationEnv::getInstance()->getCalibration()->addGroup("group1");
    CalibrationEnv::getInstance()->getCalibration()->clear();
    CalibrationEnv::getInstance()->getCalibration()->removeParameter(objectf5.getName());

    PyEnv::destroy();
    Logger() << "main finished";
    exit(1);
}
