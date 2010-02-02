#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QStringList>

class CoreEngine;
class Algorithm;

class Algorithms
{
public:
    static QStringList getAlgorithmNames();
    static Algorithm* getAlgorithmInstance(QString name, CoreEngine *engine);
};

#endif // ALGORITHMS_H
