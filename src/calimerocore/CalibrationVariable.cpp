#include <CalibrationVariable.h>
#include <string>
#include <vector>
#include <list>
#include <boost/foreach.hpp>

using namespace std;

CalibrationVariable::CalibrationVariable(string Name, vector<double> values) : Variable(Name, values, CALIBRATIONVARIABLE)
{
    initvalues=values;
    values=values;

    min = 0;
    max = 0;
}


CalibrationVariable::CalibrationVariable(const CalibrationVariable &oldvar) : Variable(oldvar)
{
    initvalues=oldvar.initvalues;
    min = oldvar.min;
    max = oldvar.max;
}

CalibrationVariable::~CalibrationVariable()
{}

vector<double> CalibrationVariable::getInitValues()
{
    return initvalues;
}

void CalibrationVariable::setInitValues(vector<double> value)
{
    if(!checkrange(value,min,max))
        return;

    initvalues.clear();
    initvalues=value;
}

bool CalibrationVariable::setValues(vector<double> value)
{
    if(!checkrange(value,min,max))
        return false;

    return Variable::setValues(value);
}

void CalibrationVariable::setMax(double value)
{
    if(value==max || value<min)
        return;

    max=value;
}

double CalibrationVariable::getMax()
{
    return max;
}

void CalibrationVariable::setMin(double value)
{
    if(value==min || value>max)
        return;

    min=value;
}

double CalibrationVariable::getMin()
{
    return min;
}

bool CalibrationVariable::checkrange(const vector<double> vec, double min, double max)
{
    BOOST_FOREACH(double value, vec)
    {
       if(value < min || value > max)
           return false;
   }

    return true;
}
