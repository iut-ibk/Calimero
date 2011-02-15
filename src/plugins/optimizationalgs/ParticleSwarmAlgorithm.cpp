#include <ParticleSwarmAlgorithm.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <Logger.h>
#include <CalibrationEnv.h>
#include <CalibrationVariable.h>
#include <IFunction.h>
#include <QPair>
#include <QVector>
#include <math.h>

CALIMERO_DECLARE_CALFUNCTION_NAME(ParticleSwarmAlgorithm)

ParticleSwarmAlgorithm::ParticleSwarmAlgorithm()
{
    setDataType("parallel",UINT,"1");
    setDataType("Swarm size", UINT, "4");
    setDataType("Max iteration number", UINT, "1000");
    setDataType("Max Error", DOUBLE, "0.1");
    setDataType("Phi-p", DOUBLE, "2");
    setDataType("Phi-g", DOUBLE, "2");
    setDataType("Omega", DOUBLE, "0.5");
    setDataType("Velocity clamping", DOUBLE, "1");
}

void ParticleSwarmAlgorithm::clean()
{
    for(int index=0; index<pvelocity.size(); index++)
        delete pvelocity[index];

    for(int index=0; index<pposition.size(); index++)
        delete pposition[index];

    for(int index=0; index<pbestposition.size(); index++)
        delete pbestposition[index];

    maxiteration=0;
    swarmsize=0;
    maxerror=0;
    clamping=0;
    omega=0;
    phig=0;
    phip=0;
    gbesthealth=0;
    initglobalhealth=true;

    gbestposition.clear();
    pbesthealth.clear();
    pvelocity.clear();
    pposition.clear();
    pbestposition.clear();
}

ParticleSwarmAlgorithm::~ParticleSwarmAlgorithm()
{
    clean();
}

bool ParticleSwarmAlgorithm::start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
{
    //remove old results
    clean();

    //extract calibration parameters
    this->calibrationpars=calibrationpars;
    this->opars=opars;
    this->env=env;
    this->calibration=calibration;
    swarmsize=atoi(getValueOfParameter("Swarm size").c_str());
    maxiteration=atoi(getValueOfParameter("Max iteration number").c_str());
    maxerror=atof(getValueOfParameter("Max Error").c_str());
    phip=atof(getValueOfParameter("Phi-p").c_str());
    phig=atof(getValueOfParameter("Phi-g").c_str());
    omega=atof(getValueOfParameter("Omega").c_str());
    clamping=atof(getValueOfParameter("Velocity clamping").c_str());

    bool error = false;

    if(omega>1.2 || omega < 0)
    {
        error = true;
        Logger(Error) << "Omega must be between 0 and 1.2";
    }

    if(phip>2 || phip < 0)
    {
        error = true;
        Logger(Error) << "Phi-p must be between 0 and 2";
    }

    if(phig>2 || phig < 0)
    {
        error = true;
        Logger(Error) << "Phi-g must be between 0 and 2";
    }

    if(clamping>1 || clamping < 0.1)
    {
        error = true;
        Logger(Error) << "Velocity clamping must be between 0.1 and 1";
    }

    if(error)
        return false;

    //init particle positions
    for(int index=0; index < swarmsize; index++)
    {
        vector<double> *position = new vector<double>(calibrationpars.size(),0);
        vector<double> *velocity = new vector<double>(calibrationpars.size(),0);

        for(int par=0; par<calibrationpars.size(); par++)
        {
            double min = calibrationpars[par]->getMin();
            double max = calibrationpars[par]->getMax();
            double step = calibrationpars[par]->getStep();

            vmax.push_back(clamping*(max-min)/2);
            (*position)[par]=randval(min,max,step);
            (*velocity)[par]=randval(-vmax[par],vmax[par],step);

        }

        pposition.push_back(position);
        pvelocity.push_back(velocity);

    }

    //create healt for each particle
    while(runStep())
    {
        //check if algorithm should terminate
        if(maxerror >= gbesthealth)
        {
            Logger(Standard) << "Max error reached. Best result: " << QString::number(gbesthealth).toStdString();
            clean();
            return true;
        }

        if(maxiteration <= calibration->getNumOfComplete())
        {
            Logger(Standard) << "Max number of iteration reached. Best result: " << QString::number(gbesthealth).toStdString();
            clean();
            return true;
        }

        //calculate new particle positions
        //Update the particle's velocity

        for(int particle=0; particle<swarmsize; particle++)
        {
            //velocity
            double rp = randval(0,1,0.00000001);
            double rg = randval(0,1,0.00000001);


            vector<double> oldvelocity(*(pvelocity[particle]));
            vector<double> newvelocity;
            vector<double> localdist = scalarMultiplication(vectorMinus(*pbestposition[particle],*pposition[particle]),phip*rp);
            vector<double> globaldist = scalarMultiplication(vectorMinus(gbestposition,*pposition[particle]),phig*rg);
            vector<double> memorycurrentstate = scalarMultiplication(oldvelocity,omega);
            newvelocity = vectorAddition(vectorAddition(memorycurrentstate,localdist),globaldist);
            delete pvelocity[particle];
            pvelocity[particle] = new vector<double>(newvelocity);

            //position
            vector<double> newposition = vectorAddition(*pposition[particle],newvelocity);
            delete pposition[particle];
            pposition[particle] = new vector<double>(positionCheck(newposition));
        }
    }

    clean();
    return false;
}


vector<double> ParticleSwarmAlgorithm::vmaxCheck(vector<double> velocity)
{
    for(int index=0; index < velocity.size(); index++)
    {
        if(velocity[index]>vmax[index])
            velocity[index]=vmax[index];

        if(velocity[index]<-vmax[index])
            velocity[index]=-vmax[index];
    }

    return velocity;
}

vector<double> ParticleSwarmAlgorithm::positionCheck(vector<double> position)
{
    for(int index=0; index < position.size(); index++)
    {
        if(position[index]>calibrationpars[index]->getMax())
            position[index]=calibrationpars[index]->getMax();

        if(position[index]<calibrationpars[index]->getMin())
            position[index]=calibrationpars[index]->getMin();

        double step = calibrationpars[index]->getStep();
        double mult = floor(position[index]/step);
        position[index]=mult*step;
    }

    return position;
}

vector<double> ParticleSwarmAlgorithm::vectorAddition(vector<double> a, vector<double> b)
{
    if(a.size()!=b.size())
        return vector<double>();

    vector<double> result(a);

    for(int index=0; index < a.size(); index++)
        result[index]+=b[index];

    return result;
}

vector<double> ParticleSwarmAlgorithm::scalarAddition(vector<double> a, double scalar)
{
    vector<double> result(a);

    for(int index=0; index < a.size(); index++)
        result[index]+=scalar;

    return result;
}

vector<double> ParticleSwarmAlgorithm::scalarMultiplication(vector<double> a, double scalar)
{
    vector<double> result(a);

    for(int index=0; index < a.size(); index++)
        result[index]*=scalar;

    return result;
}

vector<double> ParticleSwarmAlgorithm::vectorMinus(vector<double> a, vector<double> b)
{
    if(a.size()!=b.size())
        return vector<double>();

    vector<double> result(a);

    for(int index=0; index < a.size(); index++)
        result[index]-=b[index];

    return result;
}

double ParticleSwarmAlgorithm::randval(double low, double high, double step)
{
  double val;
  val = ((double)(rand()%1000)/1000.0)*(high - low) + low;
  int steps = qRound(val/step);
  return (double)steps*step;
}

bool ParticleSwarmAlgorithm::runStep()
{
    //run samples
    int completeiterations=calibration->getNumOfComplete();
    for(uint particle=0; particle < pposition.size(); particle++)
    {
        vector<double>* currentposition = pposition[particle];

        for(uint index=0; index < currentposition->size();index++)
        {
            CalibrationVariable *currentparticle = calibrationpars.at(index);
            currentparticle->setValues(vector<double>(1,currentposition->at(index)));
        }

        if(pposition.size()!=pbestposition.size())
            pbestposition.push_back(new vector<double>(*currentposition));

        if(!env->execIteration(calibrationpars))
            return false;
    }

    //wait for all particles
    env->barrier();

    //check if all results are completed

    if(completeiterations+pposition.size() < calibration->getNumOfComplete())
    {
        Logger(Error) << "Could not find all results for current swarm";
        return false;
    }

    //extract health for each particle
    vector<IterationResult * > results = calibration->getIterationResults();
    for(int parhealth=0; parhealth<pposition.size(); parhealth++)
    {
        if(opars.size() > 1)
        {
            Logger(Warning) << "This algorithm does not support multi objectives";
            return false;
        }

        IterationResult * currentresult = results.at(completeiterations+parhealth);

        double errorvalue=0.0;
        vector<double> error = currentresult->getResults(opars.at(0)->getName());
        for(int i=0; i<error.size(); i++)
            errorvalue+=error[i]*error[i];

        errorvalue=sqrt(errorvalue);

        if(pbesthealth.size()!=pposition.size())
        {
            pbesthealth.push_back(errorvalue);
            pbestposition[parhealth]=new vector<double>(*pposition[parhealth]);

            if(initglobalhealth)
            {
                initglobalhealth=false;
                gbesthealth = errorvalue;
                gbestposition = *pposition[parhealth];
            }
            else
            {
                if(gbesthealth > errorvalue)
                {
                    gbesthealth = errorvalue;
                    gbestposition = *pposition[parhealth];
                }
            }
        }
        else
        {
            if(pbesthealth[parhealth] > errorvalue)
            {
                pbesthealth[parhealth]=errorvalue;
                delete pbestposition[parhealth];
                pbestposition[parhealth]=new vector<double>(*pposition[parhealth]);

                if(gbesthealth > errorvalue)
                {
                    gbesthealth = errorvalue;
                    gbestposition = *pposition[parhealth];
                }
            }
        }
    }

    return true;
}
