#include <BruteForceSearch.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(BruteForceSearch)

BruteForceSearch::BruteForceSearch()
{
    setDataType("parallel",UINT,"1");
    setDataType("clean results", BOOL, "1");
}


BruteForceSearch::~BruteForceSearch()
{
}

bool BruteForceSearch::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{

    return bruteforcesearch(calibrationpars,0, env);
}

bool BruteForceSearch::bruteforcesearch(vector<CalibrationVariable*> calibrationpars, uint currentvar, CalibrationEnv *env)
{
    bool dirty = false;
    CalibrationVariable* var = calibrationpars.at(currentvar);
    for(double currentvalue=var->getMin(); currentvalue <= var->getMax();currentvalue=currentvalue+var->getStep())
    {
        vector<double> result;
        result.push_back(currentvalue);
        var->setValues(result);

        if(currentvar==(calibrationpars.size()-1))
        {
            if(!env->execIteration(calibrationpars))
                return false;
        }
        else
        {
            if(!bruteforcesearch(calibrationpars,currentvar+1,env))
                return true;
        }
    }
    return true;
}
