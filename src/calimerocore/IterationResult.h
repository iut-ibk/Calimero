#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

#include <vector>
#include <map>
#include <string>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <boost/shared_ptr.hpp>

class ObjectiveFunctionVariable;
class Variable;
class CalibrationVariable;
class Domain;

using namespace std;
using namespace boost;

class CALIMERO_PUBLIC IterationResult
{
private:
    int complete;
    int iterationnumber;
    map<string, vector<double> > calibrationparameters;
    map<string, vector<double> > iterationparameters;
    map<string, vector<double> > objectivefucntionparameters;
    map<string, vector<double> > observedparameters;

public:
    IterationResult(int iterationnum);
    IterationResult(int iterationnum,
                    map<string, vector<double> > calibrationparameters,
                    map<string, vector<double> > iterationparameters,
                    map<string, vector<double> > objectivefunctionparameters,
                    map<string, vector<double> > observedparameters);
    void setResults(Domain *dom);
    bool isComplete();
    int getIterationNumber();
    vector<double> getResults(string name);
    vector<double> getIterationParameterResults(string name);
    vector<double> getObservedParameterResults(string name);
    vector<double> getCalibrationParameterResults(string name);
    vector<double> getObjectiveFunctionParameterResults(string name);
    vector<string> getNamesOfObjectiveFunctionParameters();
    vector<string> getNamesOfObservedParameters();
    vector<string> getNamesOfCalibrationParameters();
    vector<string> getNamesOfIterationParameters();
};

#endif // ITERATIONRESULT_H
