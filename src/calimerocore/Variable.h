#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <CalimeroGlob.h>
#include <Domain.h>

using namespace std;
class ObjectiveFunctionVariable;

class CALIMERO_PUBLIC Variable : public Domain
{
    protected:
        set<ObjectiveFunctionVariable* > successors;
        vector<double> values;

    public:
        Variable(string Name, vector<double> value, Domain::DOMAINTYPE TYPE);
        ~Variable();
        virtual vector<double> getValues();
        virtual bool setValues(const vector<double> values);
        virtual bool addSuccessor(ObjectiveFunctionVariable* var);
        virtual bool removeSuccessor(ObjectiveFunctionVariable* var);
        virtual void fireUpdate();
};

#endif // VARIABLE_H_INCLUDED
