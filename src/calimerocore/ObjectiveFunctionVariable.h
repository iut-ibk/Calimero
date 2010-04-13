#ifndef OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
#define OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED

#include <Variable.h>
#include <IObjectiveFunction.h>
#include <string>

class ObjectiveFunctionVariable;

class CALIMERO_PUBLIC ObjectiveFunctionVariable : public Variable
{
    private:
        set<Variable*> iterationparameters;
        set<Variable*> observedparameters;
        set<ObjectiveFunctionVariable*> objectivefunctionparameters;
        bool needupdate;
        std::string function;
        std::map<string,string> functionsettings;

    public:
        ObjectiveFunctionVariable(string Name);
        ~ObjectiveFunctionVariable();
        bool addParameter(Variable* var);
        bool removeParameter(Variable* var);
        set<Variable*>* getIterationParameters();
        set<Variable*>* getObservedParameters();
        set<ObjectiveFunctionVariable*>* getObjectiveFunctionParameters();
        vector<double> getValues();
        virtual void fireUpdate();
        bool setObjectiveFunction(std::string ofunction, map<string,string> settings);
        std::string getObjectiveFunction();
        bool containsParameter(Variable* var);
        map<string,string> getObjectiveFunctionSettings();

    private:
        bool setValues(vector<double> value);
        bool parameterCycleCheck(ObjectiveFunctionVariable* var);
        bool calc();
};

#endif // OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
