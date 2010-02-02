#include "ParameterPersistenceEvaluator.h"
#include <QVector>
#include <QtDebug>

ParameterPersistenceEvaluator::ParameterPersistenceEvaluator()
{
    parameters = new QStringList();
}

ParameterPersistenceEvaluator::~ParameterPersistenceEvaluator()
{
    delete parameters;
}

bool ParameterPersistenceEvaluator::checkCompatibility(QString parameterfilestring, QString valuefilestring, QStringList *parameternames, QVector<QVector<double>* > *values)
{
    qDebug() << "ParameterPersistenceEvaluator::checkCompatibility() " << QTime::currentTime();
    parameterfilestring = parameterfilestring.simplified();
    valuefilestring = valuefilestring.simplified();

    QStringList *templateparameters = new QStringList();
    QStringList parametersplit = parameterfilestring.split("$");

    qDebug() << "ParameterPersistenceEvaluator::checkCompatibility() Splitsize=" << parametersplit.size();
    int textposition = 0;

    for(int index=0; index < parametersplit.size(); index++)
    {
        if(index%2==1)
        {
            //index should be a parametername
            //searching for parametername
            QString parametername=parametersplit.at(index);

            if(templateparameters->contains(parametername))
            {
                qWarning() << "ParameterPersistenceEvaluator::checkCompatiblity() Parametername already used [" << parametername << "] \n";
                delete templateparameters;
                return false;
            }

            //searching for double
            bool doubleok = true;
            int valuelength = 10;
            double value = 0;

            while(doubleok)
            {
                value = valuefilestring.mid(textposition,valuelength).toDouble(&doubleok);
                if(valuefilestring.length() < textposition+valuelength)
                    break;
                valuelength+=5;
            }

            while(!doubleok)
            {
                valuelength--;
                if(valuelength<1)
                {
                    qWarning() << "ParameterPersistenceEvaluator::checkCompatiblity() Parameter is not a value -> parametername: " << parametername << " string: " << valuefilestring.mid(textposition,valuelength);
                    delete templateparameters;
                    return false;
                }
                value = valuefilestring.mid(textposition,valuelength).toDouble(&doubleok);
            }

            valuelength=valuefilestring.mid(textposition,valuelength).trimmed().length();
            textposition+=valuelength;

            if(textposition>valuefilestring.length())
            {
                qWarning() << "ParameterPersistenceEvaluator::checkCompatiblity() ERROR";
                delete templateparameters;
                return false;
            }


            //check if parameter is a vector element
            bool isvectorelement=false;

            if(parameternames!=NULL)
            {
                if(parametername.contains(QRegExp(".+_.+")))
                {
                    isvectorelement=true;
                    parametername=parametername.remove(QRegExp("_.+"));
                }

                if(!isvectorelement)
                    parameternames->append(parametername);
            }

            if(isvectorelement)
            {
                QVector<double> *vec = values->at(parameternames->indexOf(parametername));
                vec->append(value);
            }
            else
            {
                if(values!=NULL)
                {
                    QVector<double> *vec = new QVector<double>(0);
                    vec->append(value);
                    values->append(vec);
                    templateparameters->append(parametername);
                }
            }


        }
        else
        {
            QString s1 = parametersplit.at(index);
            QString s2 = valuefilestring.mid(textposition,parametersplit.at(index).length());

            if(s1.length()!=s2.length())
            {
                qWarning() << "ParameterPersistenceEvaluator::checkCompatiblity() File error length: " << s1.length() << "!=" << s2.length();
                delete templateparameters;
                return false;
            }

            textposition+=parametersplit.at(index).length();
        }
    }


    delete templateparameters;
    qDebug() << "ParameterPersistenceEvaluator::checkCompatibility() DONE" << QTime::currentTime();
    return true;
}

void ParameterPersistenceEvaluator::removeParameter(QString parametername)
{
    if(parameters->contains(parametername))
        parameters->removeAt(parameters->indexOf(parametername));
}

bool ParameterPersistenceEvaluator::isFreeParameterName(QString parametername)
{
    return !parameters->contains(parametername);
}

void ParameterPersistenceEvaluator::addParameter(QString parametername)
{
    parameters->append(parametername);
}

/** @brief getCurrentValueOfParameter
  *
  * @todo: document this function
  */
bool ParameterPersistenceEvaluator::getCurrentValueOfParameter(Variable *parameter, QString parametertemplatestring, QString parametervalues)
{
    if(isFreeParameterName(parameter->getName()))
        return false;

    QStringList *parameternames = new QStringList();
    QVector<QVector<double>* > *parametervaluesvector = new QVector<QVector<double>* >(0);

    if(!checkCompatibility(parametertemplatestring, parametervalues, parameternames, parametervaluesvector))
    {
        qWarning() << "ParameterPersistenceEvaluator::getCurrentValueOfParameter() Input file is not compatible=";

        for(int index=0; index<parametervaluesvector->size(); index++)
            delete parametervaluesvector->at(index);

        delete parameternames;
        delete parametervaluesvector;
        return false;
    }

    QVector<double> value = parametervaluesvector->at(parameternames->indexOf(parameter->getName()))->mid(0);

    if(parameter->getValues()!=value)
    {
        parameter->setValues(value);
    }

    qDebug() << "ParameterPersistenceEvaluator::getCurrentValueOfParameter() veclength=" << value.size();
    for(int index=0; index<parametervaluesvector->size(); index++)
        delete parametervaluesvector->at(index);
    delete parameternames;
    delete parametervaluesvector;

    return true;
}
