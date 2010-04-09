#include <TestCalibrationAlg.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(TestCalibrationAlg)

TestCalibrationAlg::~TestCalibrationAlg()
{
}

bool TestCalibrationAlg::start()
{
    Logger(Debug) << "start called in native class \"TestCalibrationAlg\"";
    return true;
}
