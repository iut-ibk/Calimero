#ifndef EXTERNALPARAMETERREGISTRY_H
#define EXTERNALPARAMETERREGISTRY_H

#include <string>
#include <map>
#include <CalimeroGlob.h>

class Variable;

using namespace std;

class CALIMERO_PUBLIC ExternalParameterRegistry
{
private:
    //[templatename][template]
    map<string,string> regtemplates;
    //[templatename][regfilepath][parametername][Variable*]
    map<string, map<string, map<string, Variable* > > > regparameters;


public:
    ExternalParameterRegistry();
    bool registerTemplate(string name,const string &templatestring);
    bool containsTemplate(string name);
    bool containsRegFile(string templatename, string regfile);
    bool containsParameter(Variable* var, string templatename, string regfile);
    bool containsParameter(const string &name, string templatename, string regfile);
    bool importFile(string templatename, string path);
    bool registerParameter(Variable* parameter, string templatename, string regfile);
    bool updateParameters(string templatename, string regfile);
    bool deleteRegFile(string templatename, string regfile);
    bool deleteTemplate(string templatename);
    bool createValueFile(const string &templatename, const string &regfile);
    ExternalParameterRegistry* clone();
};

#endif // EXTERNALPARAMETERREGISTRY_H
