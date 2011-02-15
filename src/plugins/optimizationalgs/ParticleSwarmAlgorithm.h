#ifndef ParticleSwarmAlgorithm_H
#define ParticleSwarmAlgorithm_H

#include <ICalibrationAlg.h>
#include <vector>

class CalibrationVariable;
class ObjectiveFunctionVariable;
class CalibrationEnv;
class Calibration;

CALIMERO_DECLARE_CALFUNCTION(ParticleSwarmAlgorithm)

private:
    int maxiteration,swarmsize;
    double maxerror,omega,phig,phip,gbesthealth, clamping;
    vector<double> gbestposition,pbesthealth,vmax;
    vector<vector<double>* > pvelocity,pposition,pbestposition;
    CalibrationEnv *env;
    Calibration *calibration;
    vector<CalibrationVariable*> calibrationpars;
    vector<ObjectiveFunctionVariable*> opars;
    bool initglobalhealth;

public:
    ParticleSwarmAlgorithm();
    virtual ~ParticleSwarmAlgorithm();
    virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration);
    bool runStep();
    double randval(double low, double high, double step);
    vector<double> vectorAddition(vector<double> a, vector<double> b);
    vector<double> vectorMinus(vector<double> a, vector<double> b);
    vector<double> scalarAddition(vector<double> a, double scalar);
    vector<double> vectorMultiplication(vector<double> a, vector<double> b);
    vector<double> scalarMultiplication(vector<double> a, double scalar);
    vector<double> vmaxCheck(vector<double> velocity);
    vector<double> positionCheck(vector<double> position);
    void clean();
};

#endif // ParticleSwarmAlgorithm_H
