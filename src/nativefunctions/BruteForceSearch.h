#ifndef BRUTEFORCESEARCH_H
#define BRUTEFORCESEARCH_H

#include <ICalibrationAlg.h>

CALIMERO_DECLARE_CALFUNCTION(BruteForceSearch)
private:
        std::vector<uint> steps;
        uint maxiterations;
        CalibrationEnv *env;
public:
    BruteForceSearch();
    virtual ~BruteForceSearch();
    virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
    bool sample(int iteration,vector<CalibrationVariable*> calibrationpars);
};

#endif // BRUTEFORCESEARCH_H
