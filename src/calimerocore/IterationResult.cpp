#include <IterationResult.h>
#include <boost/foreach.hpp>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <Logger.h>
#include <Variable.h>
#include <Domain.h>
#include <assert.h>
#include <CalibrationEnv.h>
#include <Calibration.h>

IterationResult::IterationResult(int iterationnum)
{
    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    iterationnumber=iterationnum;
    complete=0;
}

IterationResult::IterationResult(int iterationnum,
                map<string, vector<double> > calibrationparameters,
                map<string, vector<double> > iterationparameters,
                map<string, vector<double> > objectivefunctionparameters,
                map<string, vector<double> > observedparameters)
{
    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    this->iterationnumber=iterationnum;
    this->calibrationparameters=calibrationparameters;
    this->iterationparameters=iterationparameters;
    this->observedparameters=observedparameters;
    this->objectivefucntionparameters=objectivefunctionparameters;
    complete = 1;
}

void IterationResult::setResults(Domain *dom)
{
    assert(!complete);

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


    Logger(Debug) << "Iteration " << iterationnumber << "is complete";
    complete=1;
}

bool IterationResult::isComplete()
{
    return complete;
}

vector<double> IterationResult::getResults(string name)
{
    assert(complete);

    vector<double> result;

    if(iterationparameters.find(name)!=iterationparameters.end())
    {
        result = getIterationParameterResults(name);
        return result;
    }

    if(observedparameters.find(name)!=observedparameters.end())
    {
        result = getObservedParameterResults(name);
        return result;
    }

    if(calibrationparameters.find(name)!=calibrationparameters.end())
    {
        result = getCalibrationParameterResults(name);
        return result;
    }

    if(objectivefucntionparameters.find(name)!=objectivefucntionparameters.end())
    {
        result = getObjectiveFunctionParameterResults(name);
        return result;
    }

    Logger(Error) << "Result container does not contain parameter [" << name << "]";
    return vector<double>();
}

vector<double> IterationResult::getIterationParameterResults(string name)
{
    assert(complete);

    if(iterationparameters.find(name)==iterationparameters.end())
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    return iterationparameters[name];
}

vector<double> IterationResult::getObservedParameterResults(string name)
{
    assert(complete);

    if(observedparameters.find(name)==observedparameters.end())
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    return observedparameters[name];
}

vector<double> IterationResult::getCalibrationParameterResults(string name)
{
    assert(complete);

    if(calibrationparameters.find(name)==calibrationparameters.end())
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    return calibrationparameters[name];
}

vector<double> IterationResult::getObjectiveFunctionParameterResults(string name)
{
    assert(complete);

    if(objectivefucntionparameters.find(name)==objectivefucntionparameters.end())
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    return objectivefucntionparameters[name];
}

int IterationResult::getIterationNumber()
{
    return iterationnumber;
}

vector<string> IterationResult::getNamesOfObjectiveFunctionParameters()
{
    vector<string> result;

    std::pair<string, vector<double> >p;
    BOOST_FOREACH(p, objectivefucntionparameters)
            result.push_back(p.first);

    return result;
}

vector<string> IterationResult::getNamesOfObservedParameters()
{
    vector<string> result;

    std::pair<string, vector<double> >p;
    BOOST_FOREACH(p, observedparameters)
            result.push_back(p.first);

    return result;
}

vector<string> IterationResult::getNamesOfCalibrationParameters()
{
    vector<string> result;

    std::pair<string, vector<double> >p;
    BOOST_FOREACH(p, calibrationparameters)
            result.push_back(p.first);

    return result;
}

vector<string> IterationResult::getNamesOfIterationParameters()
{
    vector<string> result;

    std::pair<string, vector<double> >p;
    BOOST_FOREACH(p, iterationparameters)
            result.push_back(p.first);

    return result;
}