#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <CalimeroGlob.h>

using namespace std;
class ObjectiveFunctionVariable;
class Domain;

enum  VARTYPE {CALIBRATIONVARIABLE = 0,
               OBJECTIVEFUNCTIONVARIABLE = 1,
               OBSERVEDVARIABLE = 2,
               ITERATIONVARIABLE = 3,
              };

class CALIMERO_PUBLIC Variable
{
    protected:
        set<string> successors;
        vector<double> values;
        string name;
        VARTYPE type;
        Domain* domain;
    public:
        Variable(string Name, vector<double> value, VARTYPE TYPE);
        Variable(const Variable &oldvar);
        virtual ~Variable();
        virtual vector<double> getValues();
        virtual bool setValues(const vector<double> values);
        virtual bool addSuccessor(const string &var);
        virtual bool removeSuccessor(const string &var);
        virtual void fireUpdate();
        string getName() const;
        VARTYPE getType() const;
        void setDomain(Domain* dom);
};

#endif // VARIABLE_H_INCLUDED
