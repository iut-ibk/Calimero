#ifndef EXTERNALPARAMETERREGISTRY_H
#define EXTERNALPARAMETERREGISTRY_H

#include <string>
#include <map>
#include <CalimeroGlob.h>
#include <Variable.h>

class Domain;

using namespace std;

class CALIMERO_PUBLIC ExternalParameterRegistry
{
private:
    //[templatename][template]
    map<string,string> regtemplates;
    //[templatename][path]
    map<string,string> templatepaths;
    //[parametername][templatename]
    map<string, string> regparameters;
    //[templatename][VariableType]
    map<string, VARTYPE> types;


public:
    ExternalParameterRegistry();
    ExternalParameterRegistry(const ExternalParameterRegistry &oldreg);
    bool registerTemplate(const string &name, const string &templatepath, const string &templatestring, VARTYPE type);
    bool registerParameter(const string &parametername, const string &templatename, Domain *domain);
    bool updateParameters(Domain *domain, int iteration);
    bool updateParameters(Domain *domain, const string &templatename, const string &values);
    bool deleteTemplate(const string &templatename);
    bool createValueFile(const string &templatename, Domain *domain, int iteration);
    bool createValueFile(const string &templatename, Domain *domain, const string &filepath);
    bool createvalueFiles(Domain *dom, int iteration);
    string getPath(const string &templatename);
    vector<string> getAllTemplateNames();
};

#endif // EXTERNALPARAMETERREGISTRY_H
