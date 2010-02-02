#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include <QtCore>
#include <QMutex>


class CalculationVariable;

class Variable : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString name WRITE setName READ getName )
    Q_PROPERTY( double currentvalue WRITE setCurrentValue READ getCurrentValue )

        //static const
    public:
        static const int CALIBRATIONVARIABLE = 0;
        static const int CALCULATIONVARIABLE = 1;
        static const int RESULTVARIABLE = 3;
        static const int ITERATIONVARIABLE = 4;

    protected:
        QString name;
        int type;
        QString filename;
        QVector<CalculationVariable* > monitor;
        QMutex *mutex;

    protected:
        QVector<double> values;

    public:
        Variable(QString Name, QVector<double> value, int TYPE);
        ~Variable();

        void addMonitor(CalculationVariable* var);
        void removeMonitor(CalculationVariable* var);
        void fireUpdate();

        //getter
        QString getName();
        int getType();
        QString getFileName();
        virtual double getCurrentValue();

        //Setter
        void setFileName(QString filename);
        void setName(QString name);
        void setCurrentValue(double value);
        virtual QVector<double> getValues();
        virtual void setValues(QVector<double> values);

    signals:
        void variablechanged();

};

Q_DECLARE_METATYPE(Variable*)

#endif // VARIABLE_H_INCLUDED
