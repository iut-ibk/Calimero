#ifndef GeneticAlgorithm_H
#define GeneticAlgorithm_H

#include <ICalibrationAlg.h>
#include <vector>

class CalibrationVariable;
class ObjectiveFunctionVariable;
class CalibrationEnv;
class Calibration;

CALIMERO_DECLARE_CALFUNCTION(GeneticAlgorithm)

private:
    int populationsize, maxiteration, populationcounter, duplicatebest;
    double maxerror, mutate,currentminerror;
    bool mutaterandom;
    vector<vector<vector<double>* >* >* generations;
    vector<vector<double>* >* generationhealth;
    vector<CalibrationVariable*> calibrationpars;
    vector<ObjectiveFunctionVariable*> opars;
    CalibrationEnv *env;
    Calibration *calibration;

public:
    GeneticAlgorithm();
    virtual ~GeneticAlgorithm();
    virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
    bool initialization();
    bool createGeneration();
    bool runSamples();
    void clean();
    void killGeneration(uint index);
    double randval(double low, double high, double step);
};

#endif // GeneticAlgorithm_H
