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
public:
    enum CALIBRATIONSTATE{CALIBRATIONINIT=0, CALIBRATIONRUNNING=1, CALIBRATIONSHUTDOWN=2, CALIBRATIONNOTRUNNING=3};
private:
    static Calibration* calibration;
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
    CALIBRATIONSTATE calstate;
    Registry<IObjectiveFunction>* oreg;
    Registry<IModelSimulator>* mreg;
    Registry<ICalibrationAlg>* creg;

private:
    Calibration();
    vector<CalibrationVariable*> evalCalibrationParameters();
    vector<ObjectiveFunctionVariable*> evalObjectiveParameters();

public:
    //create
    ~Calibration();
    static Calibration* getInstance();

    //calibration
    bool startCalibration();
    void stopCalibration();
    bool exec(vector<CalibrationVariable*> calibrationparameters,
              vector<Variable*> observedparameters,
              vector<Variable*> iterationparameters,
              vector<ObjectiveFunctionVariable*> objectivefunctionparameters);
    Variable* cloneParameter(Variable* old);

    //setup
    bool setCalibrationAlg(string ca);
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

    //contains
    bool containsGroup(std::string groupname);
    bool containsParameter(Variable *var);

    //getter
    int getNumOfComplete();
    vector<IterationResult*> getIterationResult();
    string getCalibrationAlg();
    Registry<IObjectiveFunction>* getObjectiveFunctionReg();
    Registry<IModelSimulator>* getModelSimulatorReg();
    Registry<ICalibrationAlg>* getCalibrationAlgReg();

    //destroy
    void clear();
    void clearIterationResults();
};

#endif // CALIBRATION_H
