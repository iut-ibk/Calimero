#include "Gaalg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QtCore>
#include "../../core/CalibrationVariable.h"
#include <time.h>

/* Global Data */
#define POPSIZE    50
#define MAXGENS    100
#define PXOVER	     0.8
#define PMUTATION    0.15
#define MAXIM        0

class genotype
{
public:
    //vars
    QVector<double> gene;
    double fitness;
    QVector<double> upper;
    QVector<double> lower;
    double rfitness;
    double cfitness;

    //methodes

    genotype(int size)
    {
        gene = QVector<double>(size);
        upper = QVector<double>(size);
        lower = QVector<double>(size);
    }

    void copy(genotype *type)
    {
        fitness=type->fitness;
        rfitness=type->rfitness;
        cfitness=type->cfitness;

        for(int index=0; index<type->gene.size();index++)
        {
            gene[index]=type->gene[index];
            upper[index]=type->upper[index];
            lower[index]=type->lower[index];
        }
    }
};

Gaalg::Gaalg(CoreEngine *e):Algorithm(e){}

bool Gaalg::init(QVector<CalibrationVariable*> calibrationparameters, QStringList *list)
{
    Q_UNUSED(list);
    engine->getModel()->clearIterationResults();
    engine->getModel()->setClearIterations(true);
    int i,j;
    srand(time(NULL));
    needobjectivefunctionresult=false;
    mem=1;
    generation=0;
    NVARS=calibrationparameters.size();

    for (j=0; j<=POPSIZE; j++)
    {
        population.append(new genotype(NVARS+1));
        newpopulation.append(new genotype(NVARS+1));
    }

    for (i=1; i<=NVARS; i++)
    {
        for (j=1; j<=POPSIZE; j++)
        {
            population[j]->fitness  = 0;
            population[j]->rfitness = 0;
            population[j]->cfitness = 0;
            population[j]->lower[i] = calibrationparameters[i-1]->getMin();
            population[j]->upper[i] = calibrationparameters[i-1]->getMax();
            population[j]->gene[i]  = randval(population[j]->lower[i],population[j]->upper[i]);
        }
    }

    return true;
}

int Gaalg::runIteration(QVector<CalibrationVariable*> calibrationparameters, bool *ok)
{
    if(needobjectivefunctionresult)
    {
        if(ok!=NULL) *ok=false;
        return -2;
    }

    if(generation>=MAXGENS)
    {
        if(ok!=NULL) *ok=false;
        return -3;
    }

    int i;
    x = new double[NVARS+1];


    /* For total population */
    if(mem>POPSIZE)
    {
        if(generation==0)keep_the_best();

        generation ++;
        select();
        crossover();
        mutate();
        mem=1;
    }

    if(mem<=POPSIZE)
    {
        for (i=1; i<=NVARS; i++)
        {
            x[i] = population[mem]->gene[i];
            calibrationparameters[i-1]->setCurrentValue(x[i]);
        }
        needobjectivefunctionresult=true;
        if(ok!=NULL) *ok=true;
        return mem;
    }
    if(ok!=NULL) *ok=false;
    return -1;
}

bool Gaalg::setIterationResult(QVector<double> result, int iterationnumber)
{
        if(!needobjectivefunctionresult)return false;
        if(result.size()==0)return false;
        if(iterationnumber<0)return false;

        population[iterationnumber]->fitness=result[0];
        mem++;
        needobjectivefunctionresult=false;
        if(mem>POPSIZE)elitist();
        return true;
}

/* ========================================================================================== */
/*
  Search for min / max  in No of entrys		1 = Max   0 = Min
  a[] values    a_n[] corresponding No of entry in array
  Give back best entry number =
    */
int Gaalg::minmax (double a[], int n, int mode)
{
    int    i;
    int    bestno;
    double best;

    if (mode){

        /* Go for max */
        bestno =   0;
        best   = -999999999;
        for(i=1;i<=n;i++){
            if (a[i] > best){
                best   = a[i];
                bestno = i;
            }
        }
    }
    else{
        /* Go for min */
        bestno =   0;
        best   =   999999999;
        for(i=1;i<=n;i++){
            if (a[i] < best){
                best   = a[i];
                bestno = i;
            }
        }
    }
    /* finish */
    return bestno;
}
/* ========================================================================================== */


/* ========================================================================================== */
double Gaalg::randval(double low, double high)
        /*
  Primitive random value generator
*/
{
    double val;
    val = ((double)(rand()%1000)/1000.0)*(high - low) + low;
    return val;
}
/* ========================================================================================== */

/* ========================================================================================== */
void Gaalg::keep_the_best(void)
        /*
  Keeps track of the best member of the population.
  A copy of the best individual is kept in the array population
  on the zero entry.
  Function is called only once - later this is handled by function elitist
*/
{
    int    i,j,best;
    double x[POPSIZE+1];

    Q_UNUSED(i);
    Q_UNUSED(j);
    /* Looks for the individual with best fitness and stores index */
    for (i=1;i<=POPSIZE;i++) x[i]=population[i]->fitness;

    if (MAXIM) {
        best = minmax(x, POPSIZE,1);
    }
    else{
        best = minmax(x, POPSIZE,0);
    }

    /* Once the best member is found, copy fitness and the genes into [0] */
    population[0]->fitness = population[best]->fitness;
    for (i=1; i<=NVARS; i++)
        population[0]->gene[i] = population[best]->gene[i];

}
/* ========================================================================================== */


/* ========================================================================================== */
void Gaalg::elitist(void)
        /*
  Best member of the previous generation is stored as the zero entry
  in the arry. If the best member of the current generation is worse
  than the array entry [1] is replaced with the stored value.
*/
{
    int    i,j,best;
    double x[POPSIZE+1];

    Q_UNUSED(j);
    /* Looks for the individual with best fitness and stores index */
    for (i=1;i<=POPSIZE;i++) x[i]=population[i]->fitness;

    if (MAXIM){
        best=minmax(x,POPSIZE,1);
        if (population[best]->fitness >= population[0]->fitness){
            for (i=1; i<=NVARS; i++)
                population[0]->gene[i] = population[best]->gene[i];
            population[0]->fitness   = population[best]->fitness;
        }
        else{
            for (i=1; i<=NVARS; i++)
                population[1]->gene[i] = population[0]->gene[i];
            population[1]->fitness   = population[0]->fitness;
        }
    }
    else{
        best=minmax(x, POPSIZE,0);
        if (population[best]->fitness <= population[0]->fitness){
            for (i=1; i<=NVARS; i++)
                population[0]->gene[i] = population[best]->gene[i];
            population[0]->fitness   = population[best]->fitness;
        }
        else{
            for (i=1; i<=NVARS; i++)
                population[1]->gene[i] = population[0]->gene[i];
            population[1]->fitness   = population[0]->fitness;
        }
    }

}
/* ========================================================================================== */

/* ========================================================================================== */
void Gaalg::select(void)
        /*
  Standard proportional selection
  Incorporating elitism
  Modification: Fitness can be positive or negative
*/
{

    int    mem,i,j,k;
    double lofit,hifit;
    double sum=0;
    double p;
    double x[POPSIZE+1];

    Q_UNUSED(k);
    /* For later search minmax */
    for (i=1;i<=POPSIZE;i++) x[i]=population[i]->fitness;


    /* calculate relative fitness of the population */
    if (MAXIM){

        /* All fitness values are shifted to positive numbers */

        /* lowest fitness value */
        lofit=population[minmax(x, POPSIZE,0)]->fitness;

        /* find total fitness of the population - shifted */
        for (mem=1; mem<=POPSIZE; mem++)
            sum += (population[mem]->fitness - lofit);

        /* standart relative fitness */
        for (mem=1; mem<=POPSIZE; mem++)
            population[mem]->rfitness=(population[mem]->fitness - lofit)/sum;
    }
    else
    {

        /* All fitness values are shifted to negative numbers */

        /* highest fitness value */
        hifit=population[minmax(x, POPSIZE,1)]->fitness;

        /* find total fitness of the population - shifted = negative */
        for (mem=1; mem<=POPSIZE; mem++)
            sum += (population[mem]->fitness - hifit);

        /* standart relative fitness */
        for (mem=1; mem<=POPSIZE; mem++)
            population[mem]->rfitness=(population[mem]->fitness - hifit)/sum;
    }

    /* calculate now cumulative fitness of the population */
    population[1]->cfitness=population[1]->rfitness;
    for (mem=2; mem<=POPSIZE; mem++)
        population[mem]->cfitness=population[mem-1]->cfitness +
                                  population[mem]->rfitness;


    /* select survivors based on cumulative fitness */
    for (i=1; i<=POPSIZE; i++){
        p=rand()%1000/1000.0;

        if (p < population[1]->cfitness)
            newpopulation[i]->copy(population[1]);
        else
        {
            if (p >= population[POPSIZE-1]->cfitness)
                newpopulation[i]->copy(population[POPSIZE]);
            else{
                for (j=1; j<POPSIZE; j++)
                    if (p >= population[j]->cfitness && p<population[j+1]->cfitness)
                        newpopulation[i]->copy(population[j+1]);
            }
        }
    }

    /* copy new population back */
    for (i=1; i<=POPSIZE; i++)
        population[i]->copy(newpopulation[i]);
}
/* ========================================================================================== */

/* ========================================================================================== */
void Gaalg::crossover(void)
{
    int i,mem,one;
    int first=0;
    double x;

    Q_UNUSED(i);

    for (mem=1; mem<=POPSIZE; ++mem)
    {
        x = rand()%1000/1000.0;
        if (x<PXOVER)
        {
            ++first;
            if (first % 2==0)
                Xover(one,mem);
            else
                one=mem;
        }
    }
}
/* ========================================================================================== */

/* ========================================================================================== */
void Gaalg::Xover(int one,int two)
{
    int i,point;

    if (NVARS>1)
    {

        point = (rand()%(NVARS-1)) +1;

        for (i=1; i<=point; i++)
            for(int index=1; index<=NVARS+1;index++)
            {
            double tmp = population[one]->gene[i];
            population[one]->gene[i]=population[two]->gene[i];
            population[two]->gene[i]=tmp;
        }
    }
}
/* ========================================================================================== */

/* ========================================================================================== */
void Gaalg::mutate(void)
{
    int i,j;
    double lbound,hbound;
    double x;

    for (i=1; i<=POPSIZE; i++)
        for (j=1; j<=NVARS; j++)
        {
        x=rand()%1000/1000.0;
        qWarning() << QString::number(x);
        if (x<PMUTATION)
        {
            lbound=population[i]->lower[j];
            hbound=population[i]->upper[j];
            population[i]->gene[j] = randval(lbound,hbound);
        }
    }
}
/* ========================================================================================== */
