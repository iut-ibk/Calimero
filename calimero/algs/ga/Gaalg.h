#ifndef GAALG_H
#define GAALG_H

#include "../Algorithm.h"

class genotype;

class Gaalg : public Algorithm
{
private:
    int 	generation;
    FILE 	*galog;
    int         NVARS;
    int         mem;
    double      *x;
    bool        needobjectivefunctionresult;
    QVector<genotype*> population;
    QVector<genotype*> newpopulation;

public:
    Gaalg(CoreEngine *engine);
    bool init(QVector<CalibrationVariable*> calibrationparameters, QStringList *list=NULL);
    int runIteration(QVector<CalibrationVariable*> calibrationparameters, bool *ok=NULL);
    bool setIterationResult(QVector<double> objectivefunctionvalues, int iterationindex);

private:
    int minmax (double a[], int n, int mode);
    double randval(double low, double high);
    void keep_the_best(void);
    void elitist(void);
    void select(void);
    void crossover(void);
    void Xover(int one,int two);
    void mutate(void);
};

#endif // GAALG_H
