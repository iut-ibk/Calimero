#ifndef DOMAIN_H
#define DOMAIN_H

#include <map>
#include <string>
#include <CalimeroGlob.h>

using namespace std;

class Variable;

class CALIMERO_PUBLIC Domain
{
public:
    enum  DOMAINTYPE {CALIBRATIONVARIABLE = 0,
                     OBJECTIVEFUNCTIONVARIABLE = 1,
                     OBSERVEDVARIABLE = 2,
                     ITERATIONVARIABLE = 3,
                     ITERATIONRESULT = 4,
                     IFUNCTION = 5,
                     CALIBRATION = 6};

private:
    string name;
    Domain * superdomain;
    map<string, Domain*> members;
    DOMAINTYPE type;
public:
    Domain(string name, DOMAINTYPE type);
    ~Domain();
    string getName() const;
    void setSuperDomain(Domain *super);
    Variable* getParameter(const string &name);
    bool contains(string var);
    bool setParameter(Variable* var);
    DOMAINTYPE getType() const;
protected:
    bool addSubDomain(Domain *sub);
    void removeSubDomain(Domain *sub);
};

#endif // DOMAIN_H
