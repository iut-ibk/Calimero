/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include <GeneticAlgorithm.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>
#include <QPair>
#include <QVector>
#include <math.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(GeneticAlgorithm)

GeneticAlgorithm::GeneticAlgorithm()
{
    setDataType("parallel",UINT,"1");
    setDataType("Population size", UINT, "100");
    setDataType("Duplicate best", UINT, "0");
    setDataType("Max iteration number", UINT, "1000000");
    setDataType("Max Error", DOUBLE, "0.1");
    setDataType("Mutate", DOUBLE, "0.01");
    setDataType("Mutate random", BOOL, "1");

    generations = new vector<vector<vector<double>* >* >();
    generationhealth = new vector<vector<double>* >();
}


GeneticAlgorithm::~GeneticAlgorithm()
{
    clean();
    delete generations;
    delete generationhealth;
}

void GeneticAlgorithm::clean()
{
    while(generations->size())
        killGeneration(generations->size()-1);
}

void GeneticAlgorithm::killGeneration(uint index)
{
    if(generations->size() <= index)
        return;

    vector<vector<double>* >* generation = generations->at(index);
    vector<double>* health;

    if(generationhealth->size() > index)
        health=generationhealth->at(index);

    if(!generation)
        return;

    while(generation->size())
    {
        generation->back();
        generation->pop_back();
    }

    delete generation;

    if(!health)
    {
        delete health;
        generationhealth->erase(generationhealth->begin()+index);
    }

    generations->erase(generations->begin()+index);

}


bool GeneticAlgorithm::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    clean();
    this->calibrationpars=calibrationpars;
    this->opars=opars;
    this->env=env;
    this->calibration=calibration;

    if(!initialization())
        return false;

    while(maxiteration > calibration->getNumOfComplete() && maxerror < currentminerror)
    {
        if(!createGeneration())
            return false;

        if(!runSamples())
            return true;
    }

    if(maxerror >= currentminerror)
        Logger(Standard) << "Best result: " << currentminerror;

    if(maxiteration <= calibration->getNumOfComplete())
        Logger(Standard) << "Max number of iteration reached";

    clean();
    return true;
}

double GeneticAlgorithm::randval(double low, double high, double step)
{
  double val;
  val = ((double)(rand()%1000)/1000.0)*(high - low) + low;
  int steps = qRound(val/step);
  return (double)steps*step;
}

bool GeneticAlgorithm::initialization()
{
    populationcounter=0;
    currentminerror=100000000000;
    //extract input data
    populationsize=atoi(getValueOfParameter("Population size").c_str());
    maxiteration=atoi(getValueOfParameter("Max iteration number").c_str());
    maxerror=atof(getValueOfParameter("Max Error").c_str());
    mutate=atof(getValueOfParameter("Mutate").c_str());
    mutaterandom=atoi(getValueOfParameter("Mutate random").c_str());
    duplicatebest=atoi(getValueOfParameter("Duplicate best").c_str());

    Logger(Standard) << "Populationsize: " << populationsize;
    Logger(Standard) << "Max iteration number: " << maxiteration;
    Logger(Standard) << "Max Error: " << QString::number(maxerror);
    Logger(Standard) << "Mutate: " << QString::number(mutate);

    //create first population
    vector<vector<double>* >* firstgen = new vector<vector<double>* >();
    generations->push_back(firstgen);

    for(int individuali=0; individuali < populationsize; individuali++)
    {
        vector<double> *individual = new vector<double>();

        for(uint index=0; index < calibrationpars.size(); index++)
        {
            CalibrationVariable *currentvar = calibrationpars.at(index);
            individual->push_back(randval(currentvar->getMin(),currentvar->getMax(),currentvar->getStep()));
        }

        firstgen->push_back(individual);
    }

    if(!runSamples())
        return false;

    return true;
}

bool GeneticAlgorithm::runSamples()
{
    uint generation = generationhealth->size();

    if(generations->size() <= generation)
    {
        Logger(Error) << "Could not find new generation";
        return false;
    }

    //run samples
    vector<vector<double>* >* currentgeneration = generations->at(generation);

    for(uint individual=0; individual < currentgeneration->size(); individual++)
    {
        vector<double>* currentgens = currentgeneration->at(individual);

        for(uint index=0; index < currentgens->size();index++)
        {
            CalibrationVariable *currentgen = calibrationpars.at(index);
            currentgen->setValues(vector<double>(1,currentgens->at(index)));
        }

        if(!env->execIteration(calibrationpars))
        {
            env->barrier();
            return false;
        }
    }

    env->barrier();

    //extract results
    vector<double>* health = new vector<double>();
    generationhealth->push_back(health);
    double besthealth=0.0;
    double worsthealth=0.0;

    vector<IterationResult * > results = calibration->getIterationResults();

    if(results.size() < (populationcounter+1)*populationsize)
    {
        Logger(Error) << "Could not find all results for current population";
        return false;
    }

    for(int individual=0; individual < populationsize; individual++)
    {
        IterationResult * currentresult = results.at(populationcounter*populationsize+individual);

        //make error sqare of all objective functions;
        double errorvalue=0.0;

        if(opars.size() > 1)
            Logger(Warning) << "This algorithm does not support multi objectives";

        for(int index=0; index < opars.size(); index++)
        {
            vector<double> error = currentresult->getResults(opars.at(index)->getName());
            for(int i=0; i<error.size(); i++)
                errorvalue+=error[i]*error[i];
        }

        health->push_back(errorvalue);

        if(errorvalue < currentminerror)
            currentminerror=sqrt(errorvalue);

        if(!individual)
        {
            besthealth=errorvalue;
            worsthealth=errorvalue;
        }
        else
        {
            if(besthealth>errorvalue)
                besthealth=errorvalue;

            if(worsthealth<errorvalue)
                worsthealth=errorvalue;
        }
    }

    //calculate inheritance propalities
    for(int index=0; index < health->size(); index++)
        if((worsthealth-besthealth))
            (*health)[index] = 1 - (health->at(index)-besthealth)/(worsthealth-besthealth);
        else
            (*health)[index] = 1;

    populationcounter++;
    return true;
}

bool GeneticAlgorithm::createGeneration()
{
    if(generations->size()!=generationhealth->size())
    {
        Logger(Error) << "number of generations != number of generationhealth: please run \"runSamples\" first";
        return false;
    }

    vector<vector<double>* >* nextgen = new vector<vector<double>* >();
    generations->push_back(nextgen);

    //find best individuum
    int bestindividual=0;

    for(int individuali=1; individuali < populationsize; individuali++)
            if(generationhealth->at(populationcounter-1)->at(bestindividual) < generationhealth->at(populationcounter-1)->at(individuali))
                bestindividual=individuali;

    if(duplicatebest<50)
        duplicatebest=50;

    int duplicates = populationsize/100.0*duplicatebest;

    for(int index=0; index<duplicates; index++)
        nextgen->push_back(generations->at(populationcounter-1)->at(bestindividual));

    QVector< QPair<int,int> > pairvector;

    for(int individuali=duplicates; individuali < populationsize; individuali++)
    {
        //search for parents
        vector<double> *father, *mother;
        bool found = false;

        while(!found)
        {
            QPair<int,int> pair;
            father=0;
            mother=0;
            while(!father)
            {
                int index = randval(0.0,populationsize-1,1);
                double propability = randval(0.0,1.0,0.000000001);

                if(generationhealth->at(populationcounter-1)->at(index) >= propability)
                {
                    pair.first=index;
                    father = generations->at(populationcounter-1)->at(index);
                }
            }

            while(!mother)
            {
                int index = randval(0.0,populationsize-1,1);
                double propability = randval(0.0,1.0,0.000000001);

                if(generationhealth->at(populationcounter-1)->at(index) >= propability && father!=generations->at(populationcounter-1)->at(index))
                {
                    pair.second=index;
                    mother = generations->at(populationcounter-1)->at(index);
                }
            }

            if(!pairvector.contains(pair))
            {
                pairvector.push_back(pair);
                found=true;
            }
        }

        vector<double> *individual = new vector<double>();

        //make child (SEX :-))
        int halfgen = father->size()/2;

        for(int genom=0; genom < father->size(); genom++)
        {
            bool domutate = false;

            if(mutate > randval(0.0,1.0,0.00001))
                domutate = true;

            if(domutate)
            {
                CalibrationVariable *currentgenom = calibrationpars.at(genom);

                if(mutaterandom)
                    individual->push_back(randval(currentgenom->getMin(),currentgenom->getMax(),currentgenom->getStep()));
                else
                {
                    double value=0.0;
                    if(genom < halfgen)
                        value=father->at(genom);
                    else
                        value=mother->at(genom);

                    if(0.5 < randval(0.0,1.0,0.0001))
                    {
                        if((value+currentgenom->getStep()) <= currentgenom->getMax() && (value+currentgenom->getStep()) >= currentgenom->getMin())
                            individual->push_back(value+currentgenom->getStep());
                        else
                            if((value-currentgenom->getStep()) <= currentgenom->getMax() && (value-currentgenom->getStep()) >= currentgenom->getMin())
                                individual->push_back(value-currentgenom->getStep());
                            else
                                individual->push_back(value);
                    }
                    else
                    {
                        if((value-currentgenom->getStep()) >= currentgenom->getMin() && (value-currentgenom->getStep()) <= currentgenom->getMax())
                            individual->push_back(mother->at(genom)-currentgenom->getStep());
                        else
                            if((value+currentgenom->getStep()) >= currentgenom->getMin() && (value+currentgenom->getStep()) <= currentgenom->getMax())
                                individual->push_back(value+currentgenom->getStep());
                            else
                                individual->push_back(value);
                    }

                }
            }
            else
            {
                if(genom < halfgen)
                    individual->push_back(father->at(genom));
                else
                    individual->push_back(mother->at(genom));
            }
        }

        nextgen->push_back(individual);
    }

    return true;
}
