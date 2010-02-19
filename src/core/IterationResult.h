#ifndef ITERATIONRESULT_H_INCLUDED
#define ITERATIONRESULT_H_INCLUDED

#include <QString>
#include <QMap>
#include <QMutex>
#include <QtCore>

class Model;

class IterationResult : public QObject
{
    Q_OBJECT

    public:
        int iterationnr;
        QMap<QString, QVector<double> > input;
        QMap<QString, QVector<double> > iteration;
        QMap<QString, QVector<double> > compare;
        bool complete;
        QMutex *mutex;

    public:
        IterationResult(Model *m, int index, bool saveall=true);
        ~IterationResult();

    public:
        QVector<double>  getValueOfCompareParameter(QString name);
        QVector<double>  getValueOfInputParameter(QString name);
        QVector<double>  getValueOfIterationParameter(QString name);
        QVector<double> getValueOfParameter(QString name);
        void updateIterationParameters(Model *m);
        bool isComplete();
        int getIterationIndex();
};

Q_DECLARE_METATYPE(IterationResult*)
Q_DECLARE_METATYPE(QVector<IterationResult*>)


#endif // ITERATIONRESULT_H_INCLUDED
