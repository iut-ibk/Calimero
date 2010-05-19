#ifndef ICALIBRATIONALG_H
#define ICALIBRATIONALG_H

#include <CalimeroGlob.h>
#include <IFunction.h>
#include <Logger.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>



#define CALIMERO_DECLARE_CALFUNCTION(function)  \
class CALIMERO_PUBLIC function : public ICalibrationAlg { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_CALFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class CalibrationEnv;
class Calibration;

class CALIMERO_PUBLIC ICalibrationAlg : public IFunction
{
    public:
        static IFUNCTIONTYPE getType()
        {
            return CALIBRATIONALGORITHM;
        };

        ICalibrationAlg(){};
        virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration) = 0;
};


#endif // ICALIBRATIONALG_H
