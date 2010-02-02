#ifndef ITERATIONRUNNABLE_H
#define ITERATIONRUNNABLE_H

#include <QRunnable>
#include <QtCore>

class CoreEngine;
class Model;
class QProcess;

class IterationRunnable : public QRunnable
{   
    public:
        CoreEngine *coreengine;
        QProcess *process;
        Model *model;
        int index;

    public:
        IterationRunnable(int index, CoreEngine *engine, Model *model);
        void run();

};

#endif // ITERATIONRUNNABLE_H
