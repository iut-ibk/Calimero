#include <ModelSimRunnable.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IterationResult.h>
#include <IModelSimulator.h>
#include <CalibrationEnv.h>

ModelSimRunnable::ModelSimRunnable(const vector<CalibrationVariable*> &calibrationparameters)
{
    //clone calibration parameters
    BOOST_FOREACH(void* p, calibrationparameters)
            this->calibrationparameters.assign(1,(CalibrationVariable*)cloneParameter((Variable*)p));
}

Variable* ModelSimRunnable::cloneParameter(Variable* old)
{
    Variable *result;

    switch(old->getType())
    {
    case Variable::ITERATIONVARIABLE:
        result = new Variable(old->getName(),old->getValues(),old->getType());
        break;
    case Variable::CALIBRATIONVARIABLE:
        result = new CalibrationVariable(old->getName(), old->getValues());
        break;
    case Variable::OBSERVEDVARIABLE:
        result = new Variable(old->getName(),old->getValues(),old->getType());
        break;
    case Variable::OBJECTIVEFUNCTIONVARIABLE:
        ObjectiveFunctionVariable *tmp = new ObjectiveFunctionVariable(old->getName());
        tmp->setObjectiveFunction(dynamic_cast<ObjectiveFunctionVariable*>(old)->getObjectiveFunction(),
                                  dynamic_cast<ObjectiveFunctionVariable*>(old)->getObjectiveFunctionSettings());
        result=tmp;
        break;
    }

    return result;
}


void ModelSimRunnable::run()
{
    //init run of one iteration
    //search for not recognized calibration parameters
    vector<CalibrationVariable*> newcal;
    set<CalibrationVariable*> allcalp = CalibrationEnv::getInstance()->getCalibration()->getAllCalibrationParameters();    
    //CalibrationVariable *oldvar = allcalp.begin();

   /* while (oldvar != allcalp.end())
    {
        CalibrationVariable *savevar;
        BOOST_FOREACH(CalibrationVariable *var , calibrationparameters)
        {
                if(var->getName()==newvar->getName())
                {
                    savevar=var;
                }
                else
                {
                    savevar=oldvar;
                    break;
                }
        }
        newcal.assign(1,savevar);
    }
*/









/*

    //create instance of model simulator
    sim = mreg->getFunction(calibration->getModelSimulator());

    std::pair<string,string> p;
    BOOST_FOREACH(p, calibration->getModelSimulatorSettings())
            tmpsim->setValueOfParameter(p.first,p.second);


    Logger(Error) << "Calibration::exec not implemented";

    //clone all parameters
    vector<CalibrationVariable*> newcalibrationparameters;
    vector<Variable*> newobservedparameters;
    vector<Variable*> newiterationparameters;
    vector<ObjectiveFunctionVariable*> newofunctions;

    BOOST_FOREACH(void* p, calibrationparameters)
            newcalibrationparameters.assign(1,(CalibrationVariable*)cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, observedparameters)
            newobservedparameters.assign(1,cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, iterationparameters)
            newiterationparameters.assign(1,cloneParameter((Variable*)p));

    BOOST_FOREACH(void* p, objectivefunctionparameters)
            newofunctions.assign(1,(ObjectiveFunctionVariable*)cloneParameter((Variable*)p));


    //link all parameters
    for(uint index = 0; index < objectivefunctionparameters.size(); index++)
    {
        //link ObjectiveFunctionVariable
        BOOST_FOREACH(ObjectiveFunctionVariable* oldofun, *(objectivefunctionparameters[index]->getObjectiveFunctionParameters()))
            BOOST_FOREACH(ObjectiveFunctionVariable* newofun, newofunctions)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }

        //link ObservedParameters
        BOOST_FOREACH(Variable* oldofun, *(objectivefunctionparameters[index]->getObservedParameters()))
            BOOST_FOREACH(Variable* newofun, observedparameters)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }

        //link IterationParameters
        BOOST_FOREACH(Variable* oldofun, *(objectivefunctionparameters[index]->getIterationParameters()))
            BOOST_FOREACH(Variable* newofun, iterationparameters)
                if(oldofun->getName()==newofun->getName())
                {
                    newofunctions[index]->addParameter(newofun);
                }
    }

*/



    if(!sim->exec(calibrationparameters,observedparameters,iterationparameters,objectivefunctionparameters,result))
        CalibrationEnv::getInstance()->stopCalibration();
}
