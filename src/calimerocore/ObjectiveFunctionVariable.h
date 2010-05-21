#ifndef OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
#define OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED

#include <Variable.h>
#include <IObjectiveFunction.h>
#include <string>
#include <QMutex>
#include <QMutexLocker>

class ObjectiveFunctionVariable;
class Domain;
class IObjectiveFunction;

class CALIMERO_PUBLIC ObjectiveFunctionVariable : public Variable
{
    private:
        set<string> iterationparameters;
        set<string> observedparameters;
        set<string> objectivefunctionparameters;
        bool needupdate;
        std::string functionname;
        QMutex mutex;
        IObjectiveFunction *function;

    public:
        ObjectiveFunctionVariable(const ObjectiveFunctionVariable &oldvar);
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
        bool parameterCycleCheck(string var);

    private:
        bool setValues(vector<double> value);
        bool calc();
};

#endif // OBJECTIVEFUNCTIONVARIABLE_H_INCLUDED
