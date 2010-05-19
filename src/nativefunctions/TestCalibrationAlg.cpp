#include <TestCalibrationAlg.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(TestCalibrationAlg)

TestCalibrationAlg::~TestCalibrationAlg()
{
}

bool TestCalibrationAlg::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    Logger(Debug) << "start called in native class \"TestCalibrationAlg\"";

    double index = 0;
    while(index < 100000)
    {
        env->execIteration(calibrationpars);
        index++;
        //sleep(1);
    }
    return false;
}
