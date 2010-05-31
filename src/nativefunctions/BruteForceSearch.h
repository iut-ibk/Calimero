#ifndef BRUTEFORCESEARCH_H
#define BRUTEFORCESEARCH_H

#include <ICalibrationAlg.h>

CALIMERO_DECLARE_CALFUNCTION(BruteForceSearch)
public:
    BruteForceSearch();
    virtual ~BruteForceSearch();
    virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
    bool bruteforcesearch(vector<CalibrationVariable*> calibrationpars, uint currentvar, CalibrationEnv *env);
};

#endif // BRUTEFORCESEARCH_H
