#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

#include <vector>
#include <map>
#include <string>
#include <CalimeroGlob.h>

class ObjectiveFunctionVariable;
class Variable;
class CalibrationVariable;
class Domain;

using namespace std;

class CALIMERO_PUBLIC IterationResult
{
private:
    bool complete;
    int iterationnumber;
    map<string, vector<double> > calibrationparameters;
    map<string, vector<double> > iterationparameters;
    map<string, vector<double> > objectivefucntionparameters;
    map<string, vector<double> > observedparameters;

public:
    IterationResult(int iterationnum);
    void setResults(Domain *dom);
    bool isComplete();
    int getIterationNumber();
    vector<double> getIterationParameterResults(string name);
    vector<double> getObservedParameterResults(string name);
    vector<double> getCalibrationParameterResults(string name);
    vector<double> getObjectiveFunctionParameterResults(string name);
};

#endif // ITERATIONRESULT_H
