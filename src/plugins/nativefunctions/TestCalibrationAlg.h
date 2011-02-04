#ifndef TESTCALIBRATIONALG_H
#define TESTCALIBRATIONALG_H

#include <ICalibrationAlg.h>

CALIMERO_DECLARE_CALFUNCTION(TestCalibrationAlg)
public:
        TestCalibrationAlg();
        virtual ~TestCalibrationAlg();
        virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
};
#endif // TESTCALIBRATIONALG_H
