#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <CalimeroGlob.h>

using namespace std;

class ObjectiveFunctionVariable;

class CALIMERO_PUBLIC Variable
{
    public:
        enum VARTYPE {CALIBRATIONVARIABLE, OBJECTIVEFUNCTIONVARIABLE, OBSERVEDVARIABLE, ITERATIONVARIABLE};

    protected:
        string name;
        VARTYPE type;
        set<ObjectiveFunctionVariable* > successors;
        vector<double> values;

    public:
        Variable(string Name, vector<double> value, VARTYPE TYPE);
        ~Variable();
        string getName() const;
        VARTYPE getType() const;
        void setName(const string name);
        virtual vector<double> getValues();
        virtual bool setValues(const vector<double> values);
        virtual bool addSuccessor(ObjectiveFunctionVariable* var);
        virtual bool removeSuccessor(ObjectiveFunctionVariable* var);
        virtual void fireUpdate();
};

#endif // VARIABLE_H_INCLUDED
