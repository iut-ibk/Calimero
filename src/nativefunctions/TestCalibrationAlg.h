#ifndef TESTCALIBRATIONALG_H
#define TESTCALIBRATIONALG_H

#include <ICalibrationAlg.h>

CALIMERO_DECLARE_CALFUNCTION(TestCalibrationAlg)
public:
        TestCalibrationAlg(){};
        virtual ~TestCalibrationAlg();
        virtual bool start();
};
#endif // TESTCALIBRATIONALG_H
