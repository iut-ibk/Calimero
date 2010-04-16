#ifndef CALIBRATIONPARAMETER_H_INCLUDED
#define CALIBRATIONPARAMETER_H_INCLUDED

#include <Variable.h>

class CALIMERO_PUBLIC CalibrationVariable : public Variable
{
    private:
        vector<double> initvalues;
        double min;
        double max;

    private:
        bool checkrange(const vector<double> vec, double min, double max);

    public:
        CalibrationVariable(string Name, vector<double> value);
        CalibrationVariable(const CalibrationVariable &oldvar);
        ~CalibrationVariable();
        vector<double> getInitValues();
        void setInitValues(vector<double> value);
        bool setValues( vector<double> vec);
        void setMax(double value);
        double getMax();
        void setMin(double value);
        double getMin();
};



#endif // CALIBRATIONPARAMETER_H_INCLUDED
