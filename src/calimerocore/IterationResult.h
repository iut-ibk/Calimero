#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

#include <vector>
#include <map>
#include <string>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <boost/shared_ptr.hpp>
#include <QStringList>

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
    QStringList calibrationparameters;
    QStringList objectivefucntionparameters;

public:
    ~IterationResult();
    IterationResult(int iterationnum);
    IterationResult(int iterationnum,
                    map<string, vector<double> > calibrationparameters,
                    map<string, vector<double> > objectivefunctionparameters);
    void setResults(Domain *dom);
    bool isComplete();
    int getIterationNumber();
    vector<double> getResults(string name);;
    vector<double> getCalibrationParameterResults(string name);
    vector<double> getObjectiveFunctionParameterResults(string name);
    vector<string> getNamesOfObjectiveFunctionParameters();
    vector<string> getNamesOfCalibrationParameters();
};

class CalimeroDB
{
private:
    static CalimeroDB* instance;
private:
    CalimeroDB();
public:
    ~CalimeroDB();
    static CalimeroDB* getInstance();
    bool saveVector(std::string name, std::vector<double> vector, int iteration);
    bool removeVector(std::string name, int iteration);
    std::vector<double> getVector(std::string name, int iteration);
};

#endif // ITERATIONRESULT_H
