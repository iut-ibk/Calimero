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

int main()
{
    using namespace boost::python;

    //Logging test
    ostream *out = &cout;
    Log::init(new OStreamLogSink(*out),Debug);
    Logger() << "starting";

    //Calibration init
    Calibration *cal = Calibration::getInstance();


    //Python test
    PyEnv *env = PyEnv::getInstance();
    env->addPythonPath("./");
    env->addPythonPath("../scripts/");
    cal->getObjectiveFunctionReg()->addNativePlugin("calimeronativefunctions.dll");
    cal->getCalibrationAlgReg()->addNativePlugin("calimeronativefunctions.dll");
    cal->getModelSimulatorReg()->addNativePlugin("calimeronativefunctions.dll");
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

    objectf.setObjectiveFunction("TestOFunction");
    objectf2.addParameter(&objectf);
    objectf3.addParameter(&objectf2);
    objectf4.addParameter(&objectf3);
    objectf5.addParameter(&objectf4);
    ObjectiveFunctionVariable *pvar = new ObjectiveFunctionVariable("pvar");
    pvar->addParameter(&objectf5);
    objectf6.addParameter(pvar);
    Variable var2("inputvar",vector<double>(4,4),Variable::ITERATIONVARIABLE);
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
    Variable *nvar = new Variable("nvar",vector<double>(2,2),Variable::ITERATIONVARIABLE);
    objectf6.addParameter(nvar);
    objectf.addParameter(nvar);
    delete nvar;
    delete pvar;
    objectf6.getValues();

    //calibration test
    cal->startCalibration();
    cal->setCalibrationAlg("TestCalibrationAlg");
    cal->startCalibration();
    cal->addDisabledGroup("test");
    cal->addParameter(&objectf5);
    cal->addParameter(&objectf5);
    cal->addGroup("group1");
    cal->clear();
    cal->removeParameter(&objectf5);

    PyEnv::destroy();
    Logger() << "main finished";
    exit(1);
}
