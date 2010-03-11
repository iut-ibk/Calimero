#include "CalibrationVariable.h"
#include <limits>
#include <math.h>

using namespace std;

CalibrationVariable::CalibrationVariable(QString Name, QVector<double> value) : Variable(Name, value, Variable::CALIBRATIONVARIABLE)
{
    initvalue=value;
    values=value;

    if(initvalue.size()>0)

    min = initvalue.at(0)-10000000;
    max = initvalue.at(0)+10000000;
}

CalibrationVariable::~CalibrationVariable()
{
}

QVector<double> CalibrationVariable::getInitValue()
{
    return initvalue;
}

void CalibrationVariable::setInitValue(QVector<double> value)
{
    for(int index=0; index<value.size(); index++)
        if(value.at(index)<min || value.at(index)>max)
        {
            qWarning() << "CalibrationVariable::setValues: Wanted to set a wrong value: Parametername: " << this->getName() << " value: " << QString::number(value.at(index));
            return;
        }

    initvalue.clear();
    initvalue+=value;
    emit variablechanged();
}

void CalibrationVariable::setValues( QVector<double> vec)
{
    QVector<double>result;
    for(int index=0; index<vec.size(); index++)
    {
        if(vec.at(index)<min || vec.at(index)>max)
        {
            qWarning() << "CalibrationVariable::setValues: Wanted to set a wrong value: Parametername: " << this->getName() << " value: " << QString::number(vec.at(index));
            if(vec.at(index)<min)
                result.append(this->getMin());

            if(vec.at(index)>min)
                result.append(this->getMax());
        }
        else
        {
            result.append(vec.at(index));
        }

        /*if(isnan(result.at(index)))
        {
            qWarning("CalibrationVariable::setValues() Calibrationalg. wanted to set NaN");
            return;
        }*/
    }

    values.clear();
    values+=result;
    emit variablechanged();
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

void CalibrationVariable::setCurrentValue(double value)
{
    if(value==Variable::getCurrentValue())
    {
        qWarning() << "CalibrationVariable::setValues: Wanted to set same value: Parametername: " << this->getName() << " value: " << QString::number(value) ;
        return;
    }

    if(value<min || value>max)
    {
        qWarning() << "CalibrationVariable::setValues: Wanted to set a wrong value: Parametername: " << this->getName() << " value: " << QString::number(value) ;
        return;
    }

    Variable::setCurrentValue(value);
}
