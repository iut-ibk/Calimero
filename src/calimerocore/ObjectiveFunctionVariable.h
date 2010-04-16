#ifndef OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
#define OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED

#include <Variable.h>
#include <IObjectiveFunction.h>
#include <string>

class ObjectiveFunctionVariable;
class Domain;

class CALIMERO_PUBLIC ObjectiveFunctionVariable : public Variable
{
    private:
        set<string> iterationparameters;
        set<string> observedparameters;
        set<string> objectivefunctionparameters;
        bool needupdate;
        std::string function;
        std::map<string,string> functionsettings;

    public:
        ObjectiveFunctionVariable(string Name);
        ~ObjectiveFunctionVariable();
        bool addParameter(const string &var);
        bool removeParameter(const string &var);
        set<string> getIterationParameters();
        set<string> getObservedParameters();
        set<string> getObjectiveFunctionParameters();
        vector<double> getValues();
        virtual void fireUpdate();
        bool setObjectiveFunction(std::string ofunction, map<string,string> settings);
        std::string getObjectiveFunction();
        bool containsParameter(const string &var);
        map<string,string> getObjectiveFunctionSettings();

    private:
        bool setValues(vector<double> value);
        bool parameterCycleCheck(ObjectiveFunctionVariable* var);
        bool calc();
};

#endif // OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
