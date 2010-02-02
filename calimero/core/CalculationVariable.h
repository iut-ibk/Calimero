#ifndef CALCULATIONPARAMETER_H_INCLUDED
#define CALCULATIONPARAMETER_H_INCLUDED

#include <QtContainerFwd>
#include <QString>
#include "Variable.h"

class CalculationVariable;
class QScriptEngine;
class QMutex;

class CalculationVariable : public Variable
{
    Q_OBJECT
    Q_PROPERTY( QString name WRITE setName READ getName )
    Q_PROPERTY( double currentvalue WRITE setCurrentValue READ getCurrentValue )

    private:
        QVector<Variable*> *iterationparameter;
        QVector<Variable*> *resultparameter;
        QVector<CalculationVariable*> *compareparameter;
        QString algname;
        bool needupdate, enableupdate;

    public:
        CalculationVariable(QString Name);
        ~CalculationVariable();
        bool parameterCycleCheck(CalculationVariable* var);
        bool addCompareParameter(CalculationVariable* var);
        bool addResultParameter(Variable* var);
        bool addIterationParameter(Variable* var);
        bool removeCompareParameter(CalculationVariable* var);
        bool removeIterationParameter(Variable* var);
        bool removeResultParameter(Variable* var);
        virtual QVector<double> getValues();
    public slots:
        int sizeOfCompareParameter();
        int sizeOfIterationParameter();
        int sizeOfResultParameter();
        CalculationVariable* atCompareParameter(int index);
        Variable* atIterationParameter(int index);
        Variable* atResultParameter(int index);
        void calc(Variable *client);
        double getCurrentValue();
        void setCurrentValue(double value);
        void setValues(QVector<double> values);
        void enableUpdate(bool enable=true);


    public:
        bool containsIterationParameter(Variable* var);
        bool containsResultParameter(Variable* var);
        bool containsCompareParameter(CalculationVariable* var);
        void setAlg(QString name);
        QString getAlg();
        QScriptEngine* getScriptEngine();
};

Q_DECLARE_METATYPE(CalculationVariable*)
Q_DECLARE_METATYPE(QVector<CalculationVariable*>)

#endif // CALCULATIONPARAMETER_H_INCLUDED
