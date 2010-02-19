#ifndef PARAMETERPERSISTENCEEVALUATOR_H_INCLUDED
#define PARAMETERPERSISTENCEEVALUATOR_H_INCLUDED

#include <QtCore>
#include "../core/Variable.h"


class ParameterPersistenceEvaluator
{
    //private atributes
    private:
        QStringList *parameters;

    //public methods
    public:
        ParameterPersistenceEvaluator();
        ~ParameterPersistenceEvaluator();
        bool checkCompatibility(QString parameterfilestring, QString valuefilestring, QStringList *parameternames=NULL, QVector<QVector<double>* > *values=NULL);
        bool isFreeParameterName(QString parametername);
        void removeParameter(QString parametername);
        void addParameter(QString parametername);
        bool getCurrentValueOfParameter(Variable *parameter, QString parametertemplatestring, QString parametervalues);
};

#endif // PARAMETERPERSISTENCEEVALUATOR_H_INCLUDED
