#ifndef DOMAIN_H
#define DOMAIN_H

#include <map>
#include <string>
#include <CalimeroGlob.h>

using namespace std;

class Variable;

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
};

#endif // DOMAIN_H
