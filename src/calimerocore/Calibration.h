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
#include <QMutex>
#include <QMutexLocker>

#define DEFAULTGROUP "default"

class CALIMERO_PUBLIC Calibration
{

private:
    map<string, string> resulthandlers;
    map<string, bool> enabledresulthandlers;
    map<string, map<string, string> > resulthandlersettings;
    set<string> calibrationparameters;
    set<string> iterationparameters;
    set<string> observedparameters;
    set<string> objectivefunctionparameters;
    map<string, bool> enabledgroups;
    map<string, bool> disabledgroups;
    map<string, set<string>* > groups;
    map<int,shared_ptr<IterationResult>  > iterationresults;
    set<string> enabledobjectivefunctionparameters;
    string alg;
    map<string,string> algsettings;
    string simulator;
    map<string,string> modelsimulatorsettings;
    Domain *domain;
    ExternalParameterRegistry *externalfilehandler;
    QMutex *mutex;

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
    bool setIterationResults(map<int,shared_ptr<IterationResult>  > iterationresults);
    bool addResultHandler(string name, string functionname, map<string,string> settings, bool enabled);
    bool removeResultHandler(string name);
    shared_ptr<IterationResult>   newIterationResult();

    //contains
    bool containsGroup(std::string groupname);
    bool containsParameter(string var);
    bool containsGroupMember(string varname, string groupname);

    //getter
    int getNumOfComplete();
    string getCalibrationAlg();
    map<string, string> getResultHandlers();
    map<string, string> getResultHandlerSettings(string name);
    bool isResultHandlerEnabled(string name);
    string getModelSimulator();
    map<string,string> getCalibrationAlgSettings();
    map<string,string> getModelSimulatorSettings();
    vector<shared_ptr<IterationResult>  > getIterationResults();
    vector<string> getAllCalibrationParameters();
    vector<string> getAllObservedParameters();
    vector<string> getAllIterationParameters();
    vector<string> getAllObjectiveFunctionParameters();
    Domain* getDomain();
    set<string> evalCalibrationParameters();
    set<string> evalObjectiveFunctionParameters();
    ExternalParameterRegistry* getExternalParameterRegistry();
    vector<string> getAllGroups();
    map<string,bool> getDisabledGroups();
    map<string,bool> getEnabledGroups();
    set<string> getGroupMembers(const string &name);
    bool isEnabledGroup(const string &name);
    bool isDisabledGroup(const string &name);

    //destroy
    void clear();
    void clearIterationResults();
};

#endif // CALIBRATION_H
