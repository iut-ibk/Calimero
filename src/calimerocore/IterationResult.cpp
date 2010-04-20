#include "IterationResult.h"
#include <boost/foreach.hpp>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <Logger.h>
#include <Variable.h>
#include <Domain.h>

IterationResult::IterationResult(int iterationnum)
{
    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    iterationnumber=iterationnum;
}

void IterationResult::setResults(Domain *dom)
{
    if(isComplete())
        return;

    vector<Variable*> tmpvec;

    //save all calibration parameters
    tmpvec = dom->getAllPars(CALIBRATIONVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
        calibrationparameters[var->getName()]=var->getValues();

    //save all iteration parameters
    tmpvec = dom->getAllPars(ITERATIONVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
        iterationparameters[var->getName()]=var->getValues();

    //save all observed parameters
    tmpvec = dom->getAllPars(OBSERVEDVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
        observedparameters[var->getName()]=var->getValues();

    //save all objective function parameters
    tmpvec = dom->getAllPars(OBJECTIVEFUNCTIONVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
        objectivefucntionparameters[var->getName()]=var->getValues();

    this->complete=true;

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
