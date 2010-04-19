#include "IterationResult.h"
#include <boost/foreach.hpp>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <Logger.h>
#include <Variable.h>

IterationResult::IterationResult(int iterationnum)
{
    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    iterationnumber=iterationnum;
}

void IterationResult::setObjectiveFunctionParameterResults(vector<ObjectiveFunctionVariable*> *results)
{
    if(complete){Logger(Warning) << "Result container already complete";return;}

    BOOST_FOREACH(ObjectiveFunctionVariable *var, *results)
            objectivefucntionparameters[var->getName()] = var->getValues();
}

void IterationResult::setCalibrationPararameterResults(vector<CalibrationVariable*> *results)
{
    if(complete){Logger(Warning) << "Result container already complete"; return;}

    BOOST_FOREACH(CalibrationVariable *var, *results)
            calibrationparameters[var->getName()] = var->getValues();
}

void IterationResult::setObservedParameterResults(vector<Variable*> *results)
{
    if(complete){Logger(Warning) << "Result container already complete"; return;}

    BOOST_FOREACH(Variable *var, *results)
            observedparameters[var->getName()] = var->getValues();

    complete=true;
}

void IterationResult::setIterationParameterResults(vector<Variable*> *results)
{
    if(complete){Logger(Warning) << "Result container already complete"; return;}

    BOOST_FOREACH(Variable *var, *results)
            iterationparameters[var->getName()] = var->getValues();
}

bool IterationResult::isComplete()
{
    return complete;
}

vector<double> IterationResult::getIterationParameterResults(string name)
{
    if(iterationparameters.find(name)==iterationparameters.end())
        Logger(Error) << "Result container does not contain parameter [" << name << "]";

    return iterationparameters[name];
}

vector<double> IterationResult::getObservedParameterResults(string name)
{
    if(observedparameters.find(name)==observedparameters.end())
        Logger(Error) << "Result container does not contain parameter [" << name << "]";

    return observedparameters[name];
}

vector<double> IterationResult::getCalibrationParameterResults(string name)
{
    if(calibrationparameters.find(name)==calibrationparameters.end())
        Logger(Error) << "Result container does not contain parameter [" << name << "]";

    return calibrationparameters[name];
}

vector<double> IterationResult::getObjectiveFunctionParameterResults(string name)
{
    if(objectivefucntionparameters.find(name)==objectivefucntionparameters.end())
        Logger(Error) << "Result container does not contain parameter [" << name << "]";

    return objectivefucntionparameters[name];
}

int IterationResult::getIterationNumber()
{
    return iterationnumber;
}
