#include "Algorithms.h"
#include "./ga/Gaalg.h"
#include "./Algorithm.h"

QStringList Algorithms::getAlgorithmNames()
{
    QStringList list;
    list.append("GA-Algorithm");
    return list;
}

Algorithm* Algorithms::getAlgorithmInstance(QString name, CoreEngine *engine)
{
    if(name=="GA-Algorithm") return dynamic_cast<Algorithm*>(new Gaalg(engine));

    return NULL;
}

