#ifndef CALIBRATIONPARAMETER_H_INCLUDED
#define CALIBRATIONPARAMETER_H_INCLUDED

#include "Variable.h"
#include <QString>
#include <QVector>

class CalibrationVariable : public Variable
{
    Q_OBJECT
    Q_PROPERTY( QVector<double> init WRITE setInitValue READ getInitValue )
    Q_PROPERTY( QVector<double> values WRITE setValues READ getValues )
    Q_PROPERTY( double min WRITE setMin READ getMin )
    Q_PROPERTY( double max WRITE setMax READ getMax )

    private:
        QVector<double> initvalue;
        double min;
        double max;

    public:
        CalibrationVariable(QString Name, QVector<double> value);
        ~CalibrationVariable();
        QVector<double> getInitValue();
        void setInitValue(QVector<double> value);
        void setValues( QVector<double> vec);
        void setMax(double value);
        double getMax();
        void setMin(double value);
        double getMin();
        void setCurrentValue(double value);
};

Q_DECLARE_METATYPE(CalibrationVariable*)
Q_DECLARE_METATYPE(QVector<double>)

#endif // CALIBRATIONPARAMETER_H_INCLUDED
