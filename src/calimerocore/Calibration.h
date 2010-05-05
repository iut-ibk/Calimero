#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <ICalibrationAlg.h>
#include <set>
#include <map>
#include <vector>
#include <Variable.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <IterationResult.h>
#include <Registry.h>
#include <IModelSimulator.h>
#include <ICalibrationAlg.h>
#include <Domain.h>
#include <ExternalParameterRegistry.h>

#define DEFAULTGROUP "default"

class CALIMERO_PUBLIC Calibration
{

private:
    set<string> calibrationparameters;
    set<string> iterationparameters;
    set<string> observedparameters;
    set<string> objectivefunctionparameters;
    map<string, bool> enabledgroups;
    map<string, bool> disabledgroups;
    map<string, set<string>* > groups;
    map<int,IterationResult*> iterationresults;
    set<string> enabledobjectivefunctionparameters;
    string alg;
    map<string,string> algsettings;
    string simulator;
    map<string,string> modelsimulatorsettings;
    Domain *domain;
    ExternalParameterRegistry *externalfilehandler;

public:
    //create
    Calibration();
    ~Calibration();

    //setup
    bool setCalibrationAlg(string ca, map<string,string> settings);
    bool setModelSimulator(string ms, map<string,string> settings);
    bool addParameter(Variable *parameter);
    bool removeParameter(string parameter);
    bool addGroup(string name);
    bool removeGroup(string name);
    bool addParameterToGroup(string var, string groupname);
    bool removeParameterFromGroup(string var, string groupname);
    bool addEnabledGroup(string groupname);
    bool removeEnabledGroup(string groupname);
    bool addDisabledGroup(string groupname);
    bool removeDisabledGroup(string groupname);
    bool addEnabledOParameter(string parameter);
    bool removeEnabledOParameter(string parameter);
    IterationResult* newIterationResult();

    //contains
    bool containsGroup(std::string groupname);
    bool containsParameter(string var);
    bool containsGroupMember(string varname, string groupname);

    //getter
    int getNumOfComplete();
    string getCalibrationAlg();
    string getModelSimulator();
    map<string,string> getCalibrationAlgSettings();
    map<string,string> getModelSimulatorSettings();
    map<int,IterationResult*> getIterationResults();
    vector<string> getAllCalibrationParameters();
    vector<string> getAllObservedParameters();
    vector<string> getAllIterationParameters();
    vector<string> getAllObjectiveFunctionParameters();
    Domain* getDomain();
    set<string> evalCalibrationParameters();
    set<string> evalObjectiveFunctionParameters();
    ExternalParameterRegistry* getExternalParameterRegistry();
    vector<string> getAllGroups();

    //destroy
    void clear();
    void clearIterationResults();
};

#endif // CALIBRATION_H
