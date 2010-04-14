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

class CALIMERO_PUBLIC Calibration
{

private:
    set<CalibrationVariable*> calibrationparameters;
    set<Variable*> iterationparameters;
    set<Variable*> observedparameters;
    set<ObjectiveFunctionVariable*> objectivefunctionparameters;
    set<set<CalibrationVariable*>* > enabledgroups;
    set<set<CalibrationVariable*>* > disabledgroups;
    map<std::string, set<CalibrationVariable*>* > groups;
    vector<IterationResult*> iterationresults;
    set<ObjectiveFunctionVariable*> enabledobjectivefunctionparameters;
    string alg;
    map<string,string> algsettings;
    string simulator;
    map<string,string> modelsimulatorsettings;

private:
    vector<CalibrationVariable*> evalCalibrationParameters();
    vector<ObjectiveFunctionVariable*> evalObjectiveParameters();

public:
    //create
    Calibration();
    ~Calibration();

    //setup
    bool setCalibrationAlg(string ca, map<string,string> settings);
    bool setModelSimulator(string ms, map<string,string> settings);
    bool addParameter(Variable *parameter);
    bool removeParameter(Variable *parameter);
    bool addGroup(std::string name);
    bool removeGroup(std::string name);
    bool addParameterToGroup(CalibrationVariable* var, std::string groupname);
    bool removeParameterFromGroup(CalibrationVariable* var, std::string groupname);
    bool addEnabledGroup(std::string groupname);
    bool removeEnabledGroup(std::string groupname);
    bool addDisabledGroup(std::string groupname);
    bool removeDisableGroup(std::string groupname);
    bool addEnabledOParameter(ObjectiveFunctionVariable* parameter);
    bool removeEnabledOParameter(ObjectiveFunctionVariable* parameter);
    IterationResult* newIterationResult();

    //contains
    bool containsGroup(std::string groupname);
    bool containsParameter(Variable *var);

    //getter
    int getNumOfComplete();
    string getCalibrationAlg();
    string getModelSimulator();
    map<string,string> getCalibrationAlgSettings();
    map<string,string> getModelSimulatorSettings();
    vector<IterationResult*> getIterationResult();
    set<CalibrationVariable*> getAllCalibrationParameters();

    //destroy
    void clear();
    void clearIterationResults();
};

#endif // CALIBRATION_H
