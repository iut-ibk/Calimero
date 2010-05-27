#include <TestCalibrationAlg.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(TestCalibrationAlg)

TestCalibrationAlg::TestCalibrationAlg()
{
    setDataType("parallel",DOUBLE,"1");
}


TestCalibrationAlg::~TestCalibrationAlg()
{
}

bool TestCalibrationAlg::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    Logger(Debug) << "start called in native class \"TestCalibrationAlg\"";

    CalibrationVariable *pi = calibrationpars[0];

    for(double index=pi->getMin(); index<pi->getMax();index=index+pi->getStep())
    {
        vector<double> result = pi->getValues();
        result[0]=index;
        pi->setValues(result);
        if(!env->execIteration(calibrationpars))
            return true;
    }

    return true;
}
