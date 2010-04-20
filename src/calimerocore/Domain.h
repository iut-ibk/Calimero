#ifndef DOMAIN_H
#define DOMAIN_H

#include <map>
#include <string>
#include <CalimeroGlob.h>
#include <Variable.h>
#include <vector>

using namespace std;

class CALIMERO_PUBLIC Domain
{
private:
    map<string, Variable*> members;
public:
    Domain();
    Domain(const Domain &olddomain);
    ~Domain();
    Variable* getPar(const string &name);
    bool contains(string var);
    void setPar(Variable* var);
    bool removePar(const string &name);
    vector<Variable*> getAllPars(const VARTYPE &type);
};

#endif // DOMAIN_H
